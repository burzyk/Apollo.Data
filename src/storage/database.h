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
  char directory[SDB_FILE_MAX_LEN];
  series_t **series;
  int max_series_count;
} database_t;

database_t *database_create(const char *directory, int max_series);
void database_destroy(database_t *db);
int database_write(database_t *db, series_id_t series_id, data_point_t *points, int count);
int database_truncate(database_t *db, series_id_t series_id);
data_point_t database_read_latest(database_t *db, series_id_t series_id);
points_reader_t *database_read(database_t *db,
                               series_id_t series_id,
                               timestamp_t begin,
                               timestamp_t end,
                               int max_points);

#endif  // SRC_STORAGE_DATABASE_H_
