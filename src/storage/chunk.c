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

#include "src/storage/chunk.h"

#include <string.h>
#include <src/diagnostics.h>
#include <inttypes.h>

#include "src/storage/disk.h"

void sdb_data_chunk_ensure_content_loaded(data_chunk_t *chunk);

int chunk_calculate_size(int points_count) {
  return points_count * sizeof(data_point_t);
}

data_chunk_t *chunk_create(const char *file_name,
                           uint64_t file_offset,
                           int max_points,
                           cache_manager_t *cache) {
  sdb_file_t *file = sdb_file_open(file_name);

  if (file == NULL) {
    return NULL;
  }

  data_chunk_t *chunk = (data_chunk_t *)sdb_alloc(sizeof(data_chunk_t));
  strncpy(chunk->file_name, file_name, SDB_FILE_MAX_LEN);
  chunk->file_offset = file_offset;
  chunk->max_points = max_points;
  chunk->cached_content = NULL;
  chunk->begin = SDB_TIMESTAMP_MAX;
  chunk->end = SDB_TIMESTAMP_MIN;
  chunk->number_of_points = 0;
  chunk->cache_manager = cache;
  chunk->cache_entry = NULL;

  size_t points_size = sizeof(data_point_t) * max_points;
  data_point_t *points = (data_point_t *)sdb_alloc(points_size);

  sdb_file_seek(file, file_offset, SEEK_SET);
  sdb_file_read(file, points, points_size);
  sdb_file_close(file);

  for (int i = 0; i < max_points && points[i].time != 0; i++) {
    chunk->begin = sdb_minl(chunk->begin, points[i].time);
    chunk->end = sdb_maxl(chunk->end, points[i].time);
    chunk->number_of_points++;
  }

  sdb_free(points);
  return chunk;
}

void chunk_destroy(data_chunk_t *chunk) {
  if (chunk->cached_content != NULL) {
    sdb_free(chunk->cached_content);
  }

  sdb_free(chunk);
}

sdb_data_points_reader_t *chunk_read(data_chunk_t *chunk, timestamp_t begin, timestamp_t end) {

  sdb_data_chunk_ensure_content_loaded(chunk);

  data_point_t begin_element = {.time=begin};
  data_point_t end_element = {.time=end};
  int count = chunk->number_of_points;
  int elem_size = sizeof(data_point_t);
  find_predicate cmp = (find_predicate)data_point_compare;

  int begin_index = sdb_find(chunk->cached_content, elem_size, count, &begin_element, cmp);
  int end_index = sdb_find(chunk->cached_content, elem_size, count, &end_element, cmp);

  int number_of_points = end_index - begin_index;
  data_point_t *points = number_of_points == 0 ? NULL : chunk->cached_content + begin_index;

  sdb_data_points_reader_t *reader = sdb_data_points_reader_create(number_of_points);
  sdb_data_points_reader_write(reader, points, number_of_points);

  cache_manager_update(chunk->cache_manager, chunk->cache_entry);

  return reader;
}

data_point_t chunk_read_latest(data_chunk_t *chunk) {

  sdb_data_chunk_ensure_content_loaded(chunk);

  data_point_t result = {.time=0, .value=0};

  if (chunk->number_of_points > 0) {
    result = chunk->cached_content[chunk->number_of_points - 1];
  }

  cache_manager_update(chunk->cache_manager, chunk->cache_entry);

  return result;
}

int chunk_write(data_chunk_t *chunk, int offset, data_point_t *points, int count) {
  if (count == 0) {
    return 0;
  }

  if (chunk->max_points < offset + count) {
    die("Trying to write outside data chunk");
  }

  sdb_file_t *file = sdb_file_open(chunk->file_name);
  sdb_file_seek(file, chunk->file_offset + offset * sizeof(data_point_t), SEEK_SET);
  size_t write_status = sdb_file_write(file, points, count * sizeof(data_point_t));
  sdb_file_close(file);

  if (!write_status) {
    return -1;
  }

  if (chunk->cached_content != NULL) {
    memcpy(chunk->cached_content + offset, points, count * sizeof(data_point_t));
  }

  if (offset == 0) {
    chunk->begin = points[0].time;
  }

  if (chunk->number_of_points <= offset + count) {
    chunk->end = points[count - 1].time;
    chunk->number_of_points = offset + count;
  }

  return 0;
}

void chunk_clean_cache(data_chunk_t *chunk) {

  if (chunk->cached_content != NULL) {
    sdb_free(chunk->cached_content);
    chunk->cached_content = NULL;
    chunk->cache_entry = NULL;

    log_debug("Chunk (%s, %" PRIu64 ", %" PRIu64 ") -> cache cleared",
              chunk->file_name,
              chunk->begin,
              chunk->end);
  }
}

void sdb_data_chunk_ensure_content_loaded(data_chunk_t *chunk) {
  if (chunk->cached_content != NULL) {
    return;
  }

  log_debug("Chunk (%s, %" PRIu64 ", %" PRIu64 ") -> loading cache",
            chunk->file_name,
            chunk->begin,
            chunk->end);

  size_t cached_content_size = sizeof(data_point_t) * chunk->max_points;
  chunk->cached_content = (data_point_t *)sdb_alloc(cached_content_size);

  if (chunk->cache_entry == NULL) {
    chunk->cache_entry = cache_manager_register_consumer(chunk->cache_manager, chunk, cached_content_size);
  }

  sdb_file_t *file = sdb_file_open(chunk->file_name);
  sdb_file_seek(file, chunk->file_offset, SEEK_SET);
  sdb_file_read(file, chunk->cached_content, cached_content_size);
  sdb_file_close(file);
}

