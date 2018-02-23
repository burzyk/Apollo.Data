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

#include "src/storage/points-reader.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

points_reader_t *points_reader_create(data_point_t *points, uint64_t points_count) {
  points_reader_t *reader = (points_reader_t *)sdb_alloc(sizeof(points_reader_t));

  reader->points_count = points_count;
  reader->points = points_count == 0
                   ? NULL
                   : (data_point_t *)sdb_alloc(points_count * sizeof(data_point_t));

  if (reader->points != NULL) {
    memcpy(reader->points, points, points_count * sizeof(data_point_t));
  }

  return reader;
}

void points_reader_destroy(points_reader_t *reader) {
  sdb_free(reader->points);
  sdb_free(reader);
}
