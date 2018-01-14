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

#ifndef SRC_STORAGE_DATABASE_H_
#define SRC_STORAGE_DATABASE_H_

#include "src/storage/data-chunk.h"
#include "src/storage/data-series.h"

typedef struct sdb_database_s {
  char _directory[SDB_FILE_MAX_LEN];
  int _points_per_chunk;

  sdb_data_series_t **_series;
  int _max_series_count;
  chunk_cache_t *_cache;
} sdb_database_t;

sdb_database_t *sdb_database_create(const char *directory,
                                    int points_per_chunk,
                                    int max_series,
                                    uint64_t soft_limit,
                                    uint64_t hard_limit);
void sdb_database_destroy(sdb_database_t *db);
int sdb_database_write(sdb_database_t *db, sdb_data_series_id_t series_id, data_point_t *points, int count);
int sdb_database_truncate(sdb_database_t *db, sdb_data_series_id_t series_id);
data_point_t sdb_database_read_latest(sdb_database_t *db, sdb_data_series_id_t series_id);
sdb_data_points_reader_t *sdb_database_read(sdb_database_t *db, sdb_data_series_id_t series_id,
                                            timestamp_t begin,
                                            timestamp_t end,
                                            int max_points);

#endif  // SRC_STORAGE_DATABASE_H_
