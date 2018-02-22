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

#ifndef SRC_STORAGE_SERIES_H_
#define SRC_STORAGE_SERIES_H_

#include "src/storage/points-reader.h"

#ifndef SDB_FILE_GROW_INCREMENT
#define SDB_FILE_GROW_INCREMENT 1000000
#endif

typedef struct series_s {
  char file_name[SDB_FILE_MAX_LEN];
  data_point_t *points;
  uint64_t points_count;
  uint64_t points_capacity;
} series_t;

series_t *series_create(const char *file_name);
void series_destroy(series_t *series);
int series_write(series_t *series, data_point_t *points, uint64_t count);
int series_truncate(series_t *series);
points_reader_t *series_read(series_t *series, timestamp_t begin, timestamp_t end, uint64_t max_points);
data_point_t series_read_latest(series_t *series);

#endif  // SRC_STORAGE_SERIES_H_
