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

#include "src/storage/series.h"

typedef struct database_s {
  char directory[SDB_STR_MAX_LEN];
  series_t **series;
  uint64_t max_series_count;
  int no_flush;
} database_t;

database_t *database_create(const char *directory, uint64_t max_series, int no_flush);
void database_destroy(database_t *db);
int database_write(database_t *db, series_id_t series_id, points_list_t *points);
int database_truncate(database_t *db, series_id_t series_id);
uint32_t database_get_point_size(database_t *db, series_id_t series_id);
points_reader_t *database_read_latest(database_t *db, series_id_t series_id);
points_reader_t *database_read(database_t *db,
                               series_id_t series_id,
                               timestamp_t begin,
                               timestamp_t end,
                               uint64_t max_points);

#endif  // SRC_STORAGE_DATABASE_H_
