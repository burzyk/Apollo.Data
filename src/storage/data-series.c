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

#include "src/utils/memory.h"
#include "src/utils/disk.h"

void sdb_data_series_register_chunk(sdb_data_series_t *series, sdb_data_chunk_t *chunk);
void sdb_data_series_delete_chunks(sdb_data_series_t *series);
sdb_data_chunk_t *sdb_data_series_create_empty_chunk(sdb_data_series_t *series);
void sdb_data_series_write_chunk(sdb_data_series_t *series,
                                 sdb_data_chunk_t *chunk,
                                 sdb_data_point_t *points,
                                 int count);
void sdb_data_series_chunk_memcpy(sdb_data_series_t *series,
                                  sdb_data_chunk_t *chunk,
                                  int position,
                                  sdb_data_point_t *points,
                                  int count);

sdb_data_series_t *sdb_data_series_create(sdb_data_series_id_t id, const char *file_name, int points_per_chunk) {
  sdb_data_series_t *series = (sdb_data_series_t *)sdb_alloc(sizeof(sdb_data_series_t));
  series->id = id;
  strncpy(series->_file_name, file_name, SDB_FILE_MAX_LEN);
  series->_points_per_chunk = points_per_chunk;
  series->_series_lock = sdb_rwlock_create();
  series->_chunks = NULL;
  series->_max_chunks = 0;
  series->_chunks_count = 0;

  int chunk_size = sdb_data_chunk_calculate_size(points_per_chunk);

  // sdb_stopwatch_t *sw = sdb_stopwatch_start();

  for (int i = 0; i < sdb_file_size(file_name) / chunk_size; i++) {
    sdb_data_chunk_t *chunk = sdb_data_chunk_create(file_name, (uint64_t)i * chunk_size, points_per_chunk);

    if (chunk != NULL) {
      sdb_data_series_register_chunk(series, chunk);
    } else {
      // TODO: (pburzynski): Improve logging : log->Info("Unable to load chunk");
    }
  }

  // log->Info("Data series loaded in: " + std::to_string(sdb_stopwatch_stop_and_destroy(sw)) + "[s]");
  return series;
}

void sdb_data_series_destroy(sdb_data_series_t *series) {
  sdb_data_series_delete_chunks(series);

  sdb_rwlock_destroy(series->_series_lock);
  sdb_free(series);
}

int sdb_data_series_write(sdb_data_series_t *series, sdb_data_point_t *points, int count) {
  sdb_rwlock_wrlock(series->_series_lock);

  if (series->_chunks_count == 0) {
    sdb_data_chunk_t *chunk = sdb_data_series_create_empty_chunk(series);
    sdb_data_series_register_chunk(series, chunk);
  }

  int first_current = 0;
  sdb_data_chunk_t *last_chunk = series->_chunks[series->_chunks_count - 1];

  while (first_current < count && points[first_current].time <= last_chunk->end) {
    first_current++;
  }

  sdb_data_series_write_chunk(series, last_chunk, points + first_current, count - first_current);

  if (first_current != 0) {
    int start = 0;
    int stop = 0;

    for (int i = 0; i < series->_chunks_count; i++) {
      sdb_data_chunk_t *chunk = series->_chunks[i];

      while (stop < first_current && points[stop].time <= chunk->end) {
        stop++;
      }

      if (stop != start) {
        sdb_data_series_write_chunk(series, chunk, points + start, stop - start);
      }

      start = stop;
    }
  }

  sdb_rwlock_unlock(series->_series_lock);

  return 0;
}

int sdb_data_series_truncate(sdb_data_series_t *series) {
  sdb_rwlock_wrlock(series->_series_lock);

  sdb_data_series_delete_chunks(series);
  int result = sdb_file_truncate(series->_file_name);

  sdb_rwlock_unlock(series->_series_lock);

  return result;
}

sdb_data_points_reader_t *sdb_data_series_read(sdb_data_series_t *series,
                                               sdb_timestamp_t begin,
                                               sdb_timestamp_t end,
                                               int max_points) {
  sdb_rwlock_rdlock(series->_series_lock);

  sdb_data_points_range_t *ranges = (sdb_data_points_range_t *)sdb_alloc(
      series->_chunks_count * sizeof(sdb_data_points_range_t));
  int ranges_count = 0;
  int total_points = 0;

  // TODO (pburzynski): refactor to binary search
  for (int i = 0; i < series->_chunks_count && total_points < max_points; i++) {
    sdb_data_chunk_t *chunk = series->_chunks[i];
    sdb_data_points_range_t range = sdb_data_chunk_read(chunk, begin, end);

    if (range.number_of_points > 0) {
      ranges[ranges_count++] = range;
      total_points += range.number_of_points;
    }
  }

  sdb_data_points_reader_t *reader = sdb_data_points_reader_create(sdb_min(max_points, total_points));

  for (int i = 0; i < ranges_count; i++) {
    sdb_data_points_reader_write(reader, ranges[i].points, ranges[i].number_of_points);
  }

  sdb_rwlock_unlock(series->_series_lock);
  sdb_free(ranges);

  return reader;
}

void sdb_data_series_register_chunk(sdb_data_series_t *series, sdb_data_chunk_t *chunk) {
  if (series->_chunks_count + 1 >= series->_max_chunks) {
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

void sdb_data_series_write_chunk(sdb_data_series_t *series,
                                 sdb_data_chunk_t *chunk,
                                 sdb_data_point_t *points,
                                 int count) {
  if (count == 0) {
    return;
  }

  if (chunk->end < points[0].time) {
    sdb_data_series_chunk_memcpy(series, chunk, chunk->number_of_points, points, count);
  } else {
    int buffer_count = count + chunk->number_of_points;
    sdb_data_points_range_t range = sdb_data_chunk_read(chunk, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
    sdb_data_point_t *buffer = (sdb_data_point_t *)sdb_alloc(buffer_count * sizeof(sdb_data_point_t));
    sdb_data_point_t *content = range.points;
    int points_pos = count - 1;
    int content_pos = range.number_of_points - 1;
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

    sdb_data_series_chunk_memcpy(series, chunk, 0, buffer + duplicated_count, buffer_count - duplicated_count);
    sdb_free(buffer);
  }
}

void sdb_data_series_chunk_memcpy(sdb_data_series_t *series,
                                  sdb_data_chunk_t *chunk,
                                  int position,
                                  sdb_data_point_t *points,
                                  int count) {
  int to_write = sdb_min(count, chunk->max_points - position);
  sdb_data_chunk_write(chunk, position, points, to_write);
  count -= to_write;
  points += to_write;

  while (count != 0) {
    chunk = sdb_data_series_create_empty_chunk(series);
    to_write = sdb_min(count, chunk->max_points);
    sdb_data_chunk_write(chunk, 0, points, to_write);
    sdb_data_series_register_chunk(series, chunk);
    count -= to_write;
    points += to_write;
  }
}

sdb_data_chunk_t *sdb_data_series_create_empty_chunk(sdb_data_series_t *series) {
  int buffer_size = series->_points_per_chunk / 2;
  void *buffer = sdb_alloc(buffer_size);
  int to_allocate = sdb_data_chunk_calculate_size(series->_points_per_chunk);

  sdb_file_t *file = sdb_file_open(series->_file_name);
  sdb_file_seek(file, 0, SEEK_END);

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
      series->_points_per_chunk);
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
