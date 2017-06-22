/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//
// Created by Pawel Burzynski on 28/01/2017.
//

#include "src/storage/data-series.h"

#include <string.h>
#include <inttypes.h>

#include "src/utils/diagnostics.h"
#include "src/utils/memory.h"
#include "src/utils/disk.h"

void sdb_data_series_register_chunk(sdb_data_series_t *series, sdb_data_chunk_t *chunk);
void sdb_data_series_delete_chunks(sdb_data_series_t *series);
sdb_data_chunk_t *sdb_data_series_create_empty_chunk(sdb_data_series_t *series);
int sdb_data_series_write_chunk(sdb_data_series_t *series,
                                sdb_data_chunk_t *chunk,
                                sdb_data_point_t *points,
                                int count);
int sdb_data_series_chunk_memcpy(sdb_data_series_t *series,
                                 sdb_data_chunk_t *chunk,
                                 int position,
                                 sdb_data_point_t *points,
                                 int count);
int sdb_data_series_write_internal(sdb_data_series_t *series, sdb_data_point_t *points, int count);
int sdb_data_series_chunk_compare_begin(sdb_timestamp_t *begin, sdb_data_chunk_t **chunk);
int sdb_data_series_chunk_compare_end(sdb_timestamp_t *end, sdb_data_chunk_t **chunk);

sdb_data_series_t *sdb_data_series_create(sdb_data_series_id_t id,
                                          const char *file_name,
                                          int points_per_chunk,
                                          sdb_cache_manager_t *cache) {
  sdb_data_series_t *series = (sdb_data_series_t *)sdb_alloc(sizeof(sdb_data_series_t));
  series->id = id;
  strncpy(series->_file_name, file_name, SDB_FILE_MAX_LEN);
  series->_points_per_chunk = points_per_chunk;
  series->_series_lock = sdb_rwlock_create();
  series->_chunks = NULL;
  series->_max_chunks = 0;
  series->_chunks_count = 0;
  series->_cache = cache;

  int chunk_size = sdb_data_chunk_calculate_size(points_per_chunk);

  for (int i = 0; i < sdb_file_size(file_name) / chunk_size; i++) {
    sdb_data_chunk_t *chunk = sdb_data_chunk_create(file_name, (uint64_t)i * chunk_size, points_per_chunk, cache);

    if (chunk != NULL) {
      sdb_data_series_register_chunk(series, chunk);
    } else {
      sdb_log_error("failed to load chunk at index: %d", i);
    }
  }

  return series;
}

void sdb_data_series_destroy(sdb_data_series_t *series) {
  sdb_data_series_delete_chunks(series);

  sdb_rwlock_destroy(series->_series_lock);
  sdb_free(series);
}

int sdb_data_series_write_internal(sdb_data_series_t *series, sdb_data_point_t *points, int count) {
  if (series->_chunks_count == 0) {
    sdb_data_chunk_t *chunk = sdb_data_series_create_empty_chunk(series);

    if (chunk == NULL) {
      sdb_log_error("failed to create initial data chunk");
      return -1;
    }

    sdb_data_series_register_chunk(series, chunk);
  }

  int first_current = 0;
  sdb_data_chunk_t *last_chunk = series->_chunks[series->_chunks_count - 1];

  while (first_current < count && points[first_current].time <= last_chunk->end) {
    first_current++;
  }

  if (sdb_data_series_write_chunk(series, last_chunk, points + first_current, count - first_current)) {
    sdb_log_error("failed to append to last chunk");
    return -1;
  }

  if (first_current != 0) {
    int start = 0;
    int stop = 0;

    for (int i = 0; i < series->_chunks_count; i++) {
      sdb_data_chunk_t *chunk = series->_chunks[i];

      while (stop < first_current && points[stop].time <= chunk->end) {
        stop++;
      }

      if (stop != start) {
        if (sdb_data_series_write_chunk(series, chunk, points + start, stop - start)) {
          sdb_log_error("failed to write to chunk: [%" PRIu64 ", %" PRIu64 ")", chunk->begin, chunk->end);
          return -1;
        }
      }

      start = stop;
    }
  }

  return 0;
}

int sdb_data_series_write(sdb_data_series_t *series, sdb_data_point_t *points, int count) {
  sdb_rwlock_wrlock(series->_series_lock);
  int result = sdb_data_series_write_internal(series, points, count);
  sdb_rwlock_unlock(series->_series_lock);

  return result;
}

int sdb_data_series_truncate(sdb_data_series_t *series) {
  sdb_rwlock_wrlock(series->_series_lock);

  sdb_data_series_delete_chunks(series);
  int result = sdb_file_truncate(series->_file_name);

  sdb_rwlock_unlock(series->_series_lock);

  return result;
}

int sdb_data_series_chunk_compare_begin(sdb_timestamp_t *begin, sdb_data_chunk_t **chunk) {
  return *begin == (*chunk)->end ? 0 : *begin < (*chunk)->end ? -1 : 1;
}

int sdb_data_series_chunk_compare_end(sdb_timestamp_t *end, sdb_data_chunk_t **chunk) {
  return *end == (*chunk)->begin ? 0 : *end < (*chunk)->begin ? -1 : 1;
}

sdb_data_points_reader_t *sdb_data_series_read(sdb_data_series_t *series,
                                               sdb_timestamp_t begin,
                                               sdb_timestamp_t end,
                                               int max_points) {
  sdb_rwlock_rdlock(series->_series_lock);

  sdb_data_points_reader_t **readers = (sdb_data_points_reader_t **)sdb_alloc(
      series->_chunks_count * sizeof(sdb_data_points_reader_t));
  int ranges_count = 0;
  int total_points = 0;

  int element_size = sizeof(sdb_data_chunk_t *);
  sdb_find_predicate begin_predicate = (sdb_find_predicate)sdb_data_series_chunk_compare_begin;
  sdb_find_predicate end_predicate = (sdb_find_predicate)sdb_data_series_chunk_compare_end;

  int begin_index = sdb_find(series->_chunks, element_size, series->_chunks_count, &begin, begin_predicate);
  int end_index = sdb_find(series->_chunks, element_size, series->_chunks_count, &end, end_predicate);

  for (int i = begin_index; i < end_index && total_points < max_points; i++) {
    sdb_data_chunk_t *chunk = series->_chunks[i];
    sdb_data_points_reader_t *r = sdb_data_chunk_read(chunk, begin, end);

    if (r->points_count > 0) {
      readers[ranges_count++] = r;
      total_points += r->points_count;
    }
  }

  sdb_data_points_reader_t *reader = sdb_data_points_reader_create(sdb_min(max_points, total_points));

  for (int i = 0; i < ranges_count; i++) {
    sdb_data_points_reader_write(reader, readers[i]->points, readers[i]->points_count);
    sdb_data_points_reader_destroy(readers[i]);
  }

  sdb_rwlock_unlock(series->_series_lock);
  sdb_free(readers);

  return reader;
}

void sdb_data_series_register_chunk(sdb_data_series_t *series, sdb_data_chunk_t *chunk) {
  if (series->_chunks_count + 1 >= series->_max_chunks) {
    sdb_log_debug("expanding chunks collection");
    series->_max_chunks += SDB_REALLOC_GROW_INCREMENT;
    series->_chunks = (sdb_data_chunk_t **)sdb_realloc(
        series->_chunks,
        series->_max_chunks * sizeof(sdb_data_chunk_t *));
  }

  int index = series->_chunks_count;

  while (index > 0 && series->_chunks[index - 1]->end > chunk->begin) {
    series->_chunks[index] = series->_chunks[index - 1];
    index--;
  }

  series->_chunks[index] = chunk;
  series->_chunks_count++;
}

int sdb_data_series_write_chunk(sdb_data_series_t *series,
                                sdb_data_chunk_t *chunk,
                                sdb_data_point_t *points,
                                int count) {
  if (count == 0) {
    return 0;
  }

  int write_result = -1;

  if (chunk->end < points[0].time) {
    write_result = sdb_data_series_chunk_memcpy(series, chunk, chunk->number_of_points, points, count);
  } else {
    int buffer_count = count + chunk->number_of_points;
    sdb_data_points_reader_t *reader = sdb_data_chunk_read(chunk, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
    sdb_data_point_t *buffer = (sdb_data_point_t *)sdb_alloc(buffer_count * sizeof(sdb_data_point_t));
    sdb_data_point_t *content = reader->points;
    int points_pos = count - 1;
    int content_pos = reader->points_count - 1;
    int duplicated_count = 0;

    for (int i = buffer_count - 1; i >= duplicated_count; i--) {
      if (points_pos < 0) {
        buffer[i] = content[content_pos--];
      } else if (content_pos < 0) {
        buffer[i] = points[points_pos--];
      } else if (points[points_pos].time < content[content_pos].time) {
        buffer[i] = content[content_pos--];
      } else if (points[points_pos].time == content[content_pos].time) {
        buffer[i] = points[points_pos--];
        content_pos--;
        duplicated_count++;
      } else {
        buffer[i] = points[points_pos--];
      }
    }

    write_result =
        sdb_data_series_chunk_memcpy(series, chunk, 0, buffer + duplicated_count, buffer_count - duplicated_count);

    sdb_free(buffer);
    sdb_data_points_reader_destroy(reader);
  }

  return write_result;
}

int sdb_data_series_chunk_memcpy(sdb_data_series_t *series,
                                 sdb_data_chunk_t *chunk,
                                 int position,
                                 sdb_data_point_t *points,
                                 int count) {
  int to_write = sdb_min(count, chunk->max_points - position);

  if (sdb_data_chunk_write(chunk, position, points, to_write)) {
    return -1;
  }

  count -= to_write;
  points += to_write;

  while (count != 0) {
    if ((chunk = sdb_data_series_create_empty_chunk(series)) == NULL) {
      return -1;
    }

    to_write = sdb_min(count, chunk->max_points);

    if (sdb_data_chunk_write(chunk, 0, points, to_write)) {
      sdb_data_chunk_destroy(chunk);
      return -1;
    }

    sdb_data_series_register_chunk(series, chunk);
    count -= to_write;
    points += to_write;
  }

  return 0;
}

sdb_data_chunk_t *sdb_data_series_create_empty_chunk(sdb_data_series_t *series) {
  sdb_file_t *file = sdb_file_open(series->_file_name);

  if (file == NULL) {
    sdb_log_error("unable to open chunk file: %s", series->_file_name);
    return NULL;
  }

  sdb_file_seek(file, 0, SEEK_END);

  int buffer_size = series->_points_per_chunk / 2;
  void *buffer = sdb_alloc(buffer_size);
  int to_allocate = sdb_data_chunk_calculate_size(series->_points_per_chunk);

  while (to_allocate > 0) {
    int to_write = sdb_min(to_allocate, buffer_size);
    sdb_file_write(file, buffer, (size_t)to_write);
    to_allocate -= to_write;
  }

  sdb_free(buffer);
  sdb_file_close(file);

  return sdb_data_chunk_create(
      series->_file_name,
      sdb_data_chunk_calculate_size(series->_points_per_chunk) * (uint64_t)series->_chunks_count,
      series->_points_per_chunk,
      series->_cache);
}

void sdb_data_series_delete_chunks(sdb_data_series_t *series) {
  if (series->_chunks == NULL) {
    return;
  }

  for (int i = 0; i < series->_chunks_count; i++) {
    sdb_data_chunk_t *chunk = series->_chunks[i];
    sdb_data_chunk_destroy(chunk);
  }

  sdb_free(series->_chunks);

  series->_chunks = NULL;
  series->_chunks_count = 0;
  series->_max_chunks = 0;
}
