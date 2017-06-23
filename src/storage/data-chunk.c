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
#include <src/utils/diagnostics.h>
#include <inttypes.h>

#include "src/utils/memory.h"
#include "src/utils/disk.h"

int sdb_data_chunk_calculate_size(int points_count) {
  return points_count * sizeof(sdb_data_point_t);
}

sdb_data_chunk_t *sdb_data_chunk_create(const char *file_name,
                                        uint64_t file_offset,
                                        int max_points,
                                        sdb_cache_manager_t *cache) {
  sdb_file_t *file = sdb_file_open(file_name);

  if (file == NULL) {
    return NULL;
  }

  sdb_data_chunk_t *chunk = (sdb_data_chunk_t *)sdb_alloc(sizeof(sdb_data_chunk_t));
  strncpy(chunk->_file_name, file_name, SDB_FILE_MAX_LEN);
  chunk->_file_offset = file_offset;
  chunk->max_points = max_points;
  chunk->_cached_content = NULL;
  chunk->begin = SDB_TIMESTAMP_MAX;
  chunk->end = SDB_TIMESTAMP_MIN;
  chunk->number_of_points = 0;
  chunk->_lock = sdb_rwlock_create();
  chunk->_cache = cache;
  chunk->_cache_entry = NULL;

  size_t points_size = sizeof(sdb_data_point_t) * max_points;
  sdb_data_point_t *points = (sdb_data_point_t *)sdb_alloc(points_size);

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
  if (chunk->_cached_content != NULL) {
    sdb_free(chunk->_cached_content);
  }

  sdb_rwlock_destroy(chunk->_lock);
  sdb_free(chunk);
}

sdb_data_points_reader_t *sdb_data_chunk_read(sdb_data_chunk_t *chunk, sdb_timestamp_t begin, sdb_timestamp_t end) {
  sdb_rwlock_rdlock(chunk->_lock);

  if (chunk->_cached_content == NULL) {
    sdb_rwlock_upgrade(chunk->_lock);

    if (chunk->_cached_content == NULL) {
      sdb_log_debug("Chunk (%s, %" PRIu64 ", %" PRIu64 ") -> loading cache",
                    chunk->_file_name,
                    chunk->begin,
                    chunk->end);

      size_t cached_content_size = sizeof(sdb_data_point_t) * chunk->max_points;
      chunk->_cached_content = (sdb_data_point_t *)sdb_alloc(cached_content_size);

      if (chunk->_cache_entry == NULL) {
        chunk->_cache_entry = sdb_cache_manager_register_consumer(chunk->_cache, chunk, cached_content_size);
      }

      sdb_file_t *file = sdb_file_open(chunk->_file_name);
      sdb_file_seek(file, chunk->_file_offset, SEEK_SET);
      sdb_file_read(file, chunk->_cached_content, cached_content_size);
      sdb_file_close(file);
    }
  }

  sdb_data_point_t begin_element = {.time=begin};
  sdb_data_point_t end_element = {.time=end};
  int count = chunk->number_of_points;
  int elem_size = sizeof(sdb_data_point_t);
  sdb_find_predicate cmp = (sdb_find_predicate)sdb_data_point_compare;

  int begin_index = sdb_find(chunk->_cached_content, elem_size, count, &begin_element, cmp);
  int end_index = sdb_find(chunk->_cached_content, elem_size, count, &end_element, cmp);

  int number_of_points = end_index - begin_index;
  sdb_data_point_t *points = number_of_points == 0 ? NULL : chunk->_cached_content + begin_index;

  sdb_data_points_reader_t *reader = sdb_data_points_reader_create(number_of_points);
  sdb_data_points_reader_write(reader, points, number_of_points);

  sdb_cache_manager_update(chunk->_cache, chunk->_cache_entry);

  sdb_rwlock_unlock(chunk->_lock);

  return reader;
}

int sdb_data_chunk_write(sdb_data_chunk_t *chunk, int offset, sdb_data_point_t *points, int count) {
  if (count == 0) {
    return 0;
  }

  sdb_rwlock_wrlock(chunk->_lock);

  if (chunk->max_points < offset + count) {
    die("Trying to write outside data chunk");
  }

  sdb_file_t *file = sdb_file_open(chunk->_file_name);
  sdb_file_seek(file, chunk->_file_offset + offset * sizeof(sdb_data_point_t), SEEK_SET);
  size_t write_status = sdb_file_write(file, points, count * sizeof(sdb_data_point_t));
  sdb_file_close(file);

  if (!write_status) {
    sdb_rwlock_unlock(chunk->_lock);
    return -1;
  }

  if (chunk->_cached_content != NULL) {
    memcpy(chunk->_cached_content + offset, points, count * sizeof(sdb_data_point_t));
  }

  if (offset == 0) {
    chunk->begin = points[0].time;
  }

  if (chunk->number_of_points <= offset + count) {
    chunk->end = points[count - 1].time;
    chunk->number_of_points = offset + count;
  }

  sdb_rwlock_unlock(chunk->_lock);
  return 0;
}

void sdb_data_chunk_clean_cache(sdb_data_chunk_t *chunk) {
  sdb_rwlock_wrlock(chunk->_lock);

  if (chunk->_cached_content != NULL) {
    sdb_free(chunk->_cached_content);
    chunk->_cached_content = NULL;
    chunk->_cache_entry = NULL;

    sdb_log_debug("Chunk (%s, %" PRIu64 ", %" PRIu64 ") -> cache cleared",
                  chunk->_file_name,
                  chunk->begin,
                  chunk->end);
  }

  sdb_rwlock_unlock(chunk->_lock);
}
