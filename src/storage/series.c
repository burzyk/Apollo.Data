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

#include "src/storage/series.h"

#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#include "src/diagnostics.h"
#include "src/storage/disk.h"

void series_register_chunk(series_t *series, chunk_t *chunk);
void series_delete_chunks(series_t *series);
chunk_t *series_create_empty_chunk(series_t *series);
int series_write_chunk(series_t *series, chunk_t *chunk, data_point_t *points, int count);
int series_chunk_memcpy(series_t *series, chunk_t *chunk, int position, data_point_t *points, int count);
int series_chunk_compare_begin(timestamp_t *begin, chunk_t **chunk);
int series_chunk_compare_end(timestamp_t *end, chunk_t **chunk);

series_t *series_create(series_id_t id, const char *file_name, int points_per_chunk, cache_manager_t *cache) {
  series_t *series = (series_t *)sdb_alloc(sizeof(series_t));
  series->id = id;
  strncpy(series->file_name, file_name, SDB_FILE_MAX_LEN);
  series->points_per_chunk = points_per_chunk;
  series->chunks = NULL;
  series->max_chunks = 0;
  series->chunks_count = 0;
  series->cache_manager = cache;

  int chunk_size = chunk_calculate_size(points_per_chunk);

  for (int i = 0; i < sdb_file_size(file_name) / chunk_size; i++) {
    chunk_t *chunk = chunk_create(file_name, (uint64_t)i * chunk_size, points_per_chunk, cache);

    if (chunk != NULL) {
      series_register_chunk(series, chunk);
    } else {
      log_error("failed to load chunk at index: %d", i);
    }
  }

  return series;
}

void series_destroy(series_t *series) {
  series_delete_chunks(series);
  sdb_free(series);
}

int series_write(series_t *series, data_point_t *points, int count) {
  // TODO: extract to a 'filter' method
  qsort(points, (size_t)count, sizeof(data_point_t), (int (*)(const void *, const void *))data_point_compare);

  int tail = -1;

  for (int i = 0; i < count; i++) {
    if (points[i].time == 0) {
      continue;
    }

    if (tail < 0 || points[tail].time < points[i].time) {
      tail++;
    }

    points[tail] = points[i];
  }

  tail++;

  if (tail != count) {
    log_info("duplicated or zero timestamp points detected in request: %d -> %d", count, tail);
  }

  count = tail;

  if (series->chunks_count == 0) {
    chunk_t *chunk = series_create_empty_chunk(series);

    if (chunk == NULL) {
      log_error("failed to create initial data chunk");
      return -1;
    }

    series_register_chunk(series, chunk);
  }

  int first_current = 0;
  chunk_t *last_chunk = series->chunks[series->chunks_count - 1];

  while (first_current < count && points[first_current].time <= last_chunk->end) {
    first_current++;
  }

  if (series_write_chunk(series, last_chunk, points + first_current, count - first_current)) {
    log_error("failed to append to last chunk");
    return -1;
  }

  if (first_current != 0) {
    int start = 0;
    int stop = 0;

    for (int i = 0; i < series->chunks_count; i++) {
      chunk_t *chunk = series->chunks[i];

      while (stop < first_current && points[stop].time <= chunk->end) {
        stop++;
      }

      if (stop != start) {
        if (series_write_chunk(series, chunk, points + start, stop - start)) {
          log_error("failed to write to chunk: [%" PRIu64 ", %" PRIu64 ")", chunk->begin, chunk->end);
          return -1;
        }
      }

      start = stop;
    }
  }

  return 0;
}

int series_truncate(series_t *series) {
  series_delete_chunks(series);
  return sdb_file_truncate(series->file_name);
}

int series_chunk_compare_begin(timestamp_t *begin, chunk_t **chunk) {
  return *begin == (*chunk)->end ? 0 : *begin < (*chunk)->end ? -1 : 1;
}

int series_chunk_compare_end(timestamp_t *end, chunk_t **chunk) {
  return *end == (*chunk)->begin ? 0 : *end < (*chunk)->begin ? -1 : 1;
}

points_reader_t *series_read(series_t *series, timestamp_t begin, timestamp_t end, int max_points) {
  points_reader_t **readers = (points_reader_t **)sdb_alloc(
      series->chunks_count * sizeof(points_reader_t));
  int ranges_count = 0;
  int total_points = 0;

  int element_size = sizeof(chunk_t *);
  find_predicate begin_predicate = (find_predicate)series_chunk_compare_begin;
  find_predicate end_predicate = (find_predicate)series_chunk_compare_end;

  int begin_index = sdb_find(series->chunks, element_size, series->chunks_count, &begin, begin_predicate);
  int end_index = sdb_find(series->chunks, element_size, series->chunks_count, &end, end_predicate);

  for (int i = begin_index; i < end_index && total_points < max_points; i++) {
    chunk_t *chunk = series->chunks[i];
    points_reader_t *r = chunk_read(chunk, begin, end);

    if (r->points_count > 0) {
      readers[ranges_count++] = r;
      total_points += r->points_count;
    } else {
      points_reader_destroy(r);
    }
  }

  points_reader_t *reader = points_reader_create(sdb_min(max_points, total_points));

  for (int i = 0; i < ranges_count; i++) {
    points_reader_write(reader, readers[i]->points, readers[i]->points_count);
    points_reader_destroy(readers[i]);
  }

  sdb_free(readers);

  return reader;
}

data_point_t series_read_latest(series_t *series) {
  data_point_t result = {.value=0, .time=0};

  if (series->chunks_count > 0) {
    chunk_t *last_chunk = series->chunks[series->chunks_count - 1];
    result = chunk_read_latest(last_chunk);
  }

  return result;
}

void series_register_chunk(series_t *series, chunk_t *chunk) {
  if (series->chunks_count + 1 >= series->max_chunks) {
    log_debug("expanding chunks collection");
    series->max_chunks += SDB_REALLOC_GROW_INCREMENT;
    series->chunks = (chunk_t **)sdb_realloc(
        series->chunks,
        series->max_chunks * sizeof(chunk_t *));
  }

  int index = series->chunks_count;

  while (index > 0 && series->chunks[index - 1]->end > chunk->begin) {
    series->chunks[index] = series->chunks[index - 1];
    index--;
  }

  series->chunks[index] = chunk;
  series->chunks_count++;
}

int series_write_chunk(series_t *series, chunk_t *chunk, data_point_t *points, int count) {
  if (count == 0) {
    return 0;
  }

  int write_result = -1;

  if (chunk->end < points[0].time) {
    write_result = series_chunk_memcpy(series, chunk, chunk->number_of_points, points, count);
  } else {
    int buffer_count = count + chunk->number_of_points;
    points_reader_t *reader = chunk_read(chunk, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
    data_point_t *buffer = (data_point_t *)sdb_alloc(buffer_count * sizeof(data_point_t));
    data_point_t *content = reader->points;
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

    write_result = series_chunk_memcpy(series, chunk, 0, buffer + duplicated_count, buffer_count - duplicated_count);

    sdb_free(buffer);
    points_reader_destroy(reader);
  }

  return write_result;
}

int series_chunk_memcpy(series_t *series, chunk_t *chunk, int position, data_point_t *points, int count) {
  int to_write = sdb_min(count, chunk->max_points - position);

  if (chunk_write(chunk, position, points, to_write)) {
    return -1;
  }

  count -= to_write;
  points += to_write;

  while (count != 0) {
    if ((chunk = series_create_empty_chunk(series)) == NULL) {
      return -1;
    }

    to_write = sdb_min(count, chunk->max_points);

    if (chunk_write(chunk, 0, points, to_write)) {
      chunk_destroy(chunk);
      return -1;
    }

    series_register_chunk(series, chunk);
    count -= to_write;
    points += to_write;
  }

  return 0;
}

chunk_t *series_create_empty_chunk(series_t *series) {
  sdb_file_t *file = sdb_file_open(series->file_name);

  if (file == NULL) {
    log_error("unable to open chunk file: %s", series->file_name);
    return NULL;
  }

  sdb_file_seek(file, 0, SEEK_END);

  int buffer_size = series->points_per_chunk / 2;
  void *buffer = sdb_alloc(buffer_size);
  int to_allocate = chunk_calculate_size(series->points_per_chunk);

  while (to_allocate > 0) {
    int to_write = sdb_min(to_allocate, buffer_size);
    sdb_file_write(file, buffer, (size_t)to_write);
    to_allocate -= to_write;
  }

  sdb_free(buffer);
  sdb_file_close(file);

  return chunk_create(
      series->file_name,
      chunk_calculate_size(series->points_per_chunk) * (uint64_t)series->chunks_count,
      series->points_per_chunk,
      series->cache_manager);
}

void series_delete_chunks(series_t *series) {
  if (series->chunks == NULL) {
    return;
  }

  for (int i = 0; i < series->chunks_count; i++) {
    chunk_t *chunk = series->chunks[i];
    chunk_destroy(chunk);
  }

  sdb_free(series->chunks);

  series->chunks = NULL;
  series->chunks_count = 0;
  series->max_chunks = 0;
}
