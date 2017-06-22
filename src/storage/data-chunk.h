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

#ifndef SRC_STORAGE_DATA_CHUNK_H_
#define SRC_STORAGE_DATA_CHUNK_H_

#include "src/common.h"
#include "src/utils/threading.h"
#include "src/storage/data-points-reader.h"

typedef struct sdb_data_chunk_s {
  sdb_timestamp_t begin;
  sdb_timestamp_t end;
  int number_of_points;
  int max_points;

  char _file_name[SDB_FILE_MAX_LEN];
  uint64_t _file_offset;
  sdb_data_point_t *_cached_content;
  sdb_rwlock_t *_lock;
} sdb_data_chunk_t;

int sdb_data_chunk_calculate_size(int points_count);
sdb_data_chunk_t *sdb_data_chunk_create(const char *file_name, uint64_t file_offset, int max_points);
void sdb_data_chunk_destroy(sdb_data_chunk_t *chunk);
sdb_data_points_reader_t *sdb_data_chunk_read(sdb_data_chunk_t *chunk, sdb_timestamp_t begin, sdb_timestamp_t end);
int sdb_data_chunk_write(sdb_data_chunk_t *chunk, int offset, sdb_data_point_t *points, int count);

#endif  // SRC_STORAGE_DATA_CHUNK_H_
