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

#ifndef SRC_STORAGE_CHUNK_H_
#define SRC_STORAGE_CHUNK_H_

#include "src/common.h"
#include "src/storage/points-reader.h"
#include "src/storage/cache-manager.h"

typedef struct data_chunk_s {
  timestamp_t begin;
  timestamp_t end;
  int number_of_points;
  int max_points;

  char file_name[SDB_FILE_MAX_LEN];
  uint64_t file_offset;
  data_point_t *cached_content;

  cache_manager_t *cache_manager;
  cache_entry_t *cache_entry;
} data_chunk_t;

int chunk_calculate_size(int points_count);

data_chunk_t *chunk_create(const char *file_name, uint64_t file_offset, int max_points, cache_manager_t *cache);
void chunk_destroy(data_chunk_t *chunk);
points_reader_t *chunk_read(data_chunk_t *chunk, timestamp_t begin, timestamp_t end);
data_point_t chunk_read_latest(data_chunk_t *chunk);
int chunk_write(data_chunk_t *chunk, int offset, data_point_t *points, int count);
void chunk_clean_cache(data_chunk_t *chunk);

#endif  // SRC_STORAGE_CHUNK_H_
