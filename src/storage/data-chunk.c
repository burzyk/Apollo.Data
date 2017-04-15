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
// Created by Pawel Burzynski on 17/01/2017.
//

#include "src/storage/data-chunk.h"

#include <string.h>

#include "src/utils/memory.h"
#include "src/utils/disk.h"

int sdb_data_chunk_calculate_size(int points_count) {
  return points_count * sizeof(sdb_data_point_t);
}

sdb_data_chunk_t *sdb_data_chunk_create(const char *file_name, uint64_t file_offset, int max_points) {
  sdb_data_chunk_t *chunk = (sdb_data_chunk_t *)sdb_alloc(sizeof(sdb_data_chunk_t));
  strncpy(chunk->file_name, file_name, SDB_FILE_MAX_LEN);
  chunk->file_offset = file_offset;
  chunk->max_points = max_points;
  chunk->cached_content = NULL;
  chunk->begin = SDB_TIMESTAMP_MAX;
  chunk->end = SDB_TIMESTAMP_MIN;
  chunk->number_of_points = 0;
  chunk->lock = sdb_rwlock_create();

  size_t points_size = sizeof(sdb_data_point_t) * max_points;
  sdb_data_point_t *points = (sdb_data_point_t *)sdb_alloc(points_size);

  sdb_file_t *file = sdb_file_open(file_name);

  if (file == NULL) {
    return NULL;
  }

  sdb_file_seek(file, file_offset, SEEK_SET);
  sdb_file_read(file, points, points_size);
  sdb_file_close(file);

  for (int i = 0; i < max_points && points[i].time != 0; i++) {
    chunk->begin = sdb_min(chunk->begin, points[i].time);
    chunk->end = sdb_max(chunk->end, points[i].time);
    chunk->number_of_points++;
  }

  sdb_free(points);
  return chunk;
}

void sdb_data_chunk_destroy(sdb_data_chunk_t *chunk) {
  if (chunk->cached_content != NULL) {
    sdb_free(chunk->cached_content);
  }

  sdb_rwlock_destroy(chunk->lock);
  sdb_free(chunk);
}

sdb_data_points_range_t sdb_data_chunk_read(sdb_data_chunk_t *chunk, sdb_timestamp_t begin, sdb_timestamp_t end) {
  sdb_rwlock_rdlock(chunk->lock);

  if (chunk->cached_content == NULL) {
    sdb_rwlock_upgrade(chunk->lock);

    if (chunk->cached_content == NULL) {
      size_t cached_content_size = sizeof(sdb_data_point_t) * chunk->max_points;
      chunk->cached_content = (sdb_data_point_t *)sdb_alloc(cached_content_size);

      sdb_file_t *file = sdb_file_open(chunk->file_name);
      sdb_file_seek(file, chunk->file_offset, SEEK_SET);
      sdb_file_read(file, chunk->cached_content, cached_content_size);
      sdb_file_close(file);
    }
  }

  sdb_data_points_range_t range = {.points = NULL, .number_of_points = 0};

  // TODO (pburzynski): refactor to binary search and check chunk begin and end
  for (int i = 0; i < chunk->number_of_points; i++) {
    if (begin <= chunk->cached_content[i].time && chunk->cached_content[i].time < end) {
      if (range.points == NULL) {
        range.points = chunk->cached_content + i;
      }

      range.number_of_points++;
    }
  }

  sdb_rwlock_unlock(chunk->lock);
  return range;
}

void sdb_data_chunk_write(sdb_data_chunk_t *chunk, int offset, sdb_data_point_t *points, int count) {
  if (count == 0) {
    return;
  }

  sdb_rwlock_wrlock(chunk->lock);

  if (chunk->max_points < offset + count) {
    die("Trying to write outside data chunk");
  }

  sdb_file_t *file = sdb_file_open(chunk->file_name);
  sdb_file_seek(file, chunk->file_offset + offset * sizeof(sdb_data_point_t), SEEK_SET);
  sdb_file_write(file, points, count * sizeof(sdb_data_point_t));
  sdb_file_close(file);

  if (chunk->cached_content != NULL) {
    memcpy(chunk->cached_content + offset, points, count * sizeof(sdb_data_point_t));
  }

  if (offset == 0) {
    chunk->begin = points[0].time;
  }

  if (chunk->number_of_points <= offset + count) {
    chunk->end = points[count - 1].time;
    chunk->number_of_points = offset + count;
  }

  sdb_rwlock_unlock(chunk->lock);
}
