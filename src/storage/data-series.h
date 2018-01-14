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

#ifndef SRC_STORAGE_DATA_SERIES_H_
#define SRC_STORAGE_DATA_SERIES_H_

#include "src/storage/data-chunk.h"
#include "src/storage/data-points-reader.h"

typedef struct sdb_data_series_s {
  sdb_data_series_id_t id;

  char _file_name[SDB_FILE_MAX_LEN];
  int _points_per_chunk;

  sdb_data_chunk_t **_chunks;
  int _chunks_count;
  int _max_chunks;
  chunk_cache_t *_cache;
} sdb_data_series_t;

sdb_data_series_t *sdb_data_series_create(sdb_data_series_id_t id,
                                          const char *file_name,
                                          int points_per_chunk,
                                          chunk_cache_t *cache);
void sdb_data_series_destroy(sdb_data_series_t *series);
int sdb_data_series_write(sdb_data_series_t *series, data_point_t *points, int count);
int sdb_data_series_truncate(sdb_data_series_t *series);
sdb_data_points_reader_t *sdb_data_series_read(sdb_data_series_t *series,
                                               timestamp_t begin,
                                               timestamp_t end,
                                               int max_points);
data_point_t sdb_data_series_read_latest(sdb_data_series_t *series);

#endif  // SRC_STORAGE_DATA_SERIES_H_
