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

#include "src/storage/data-points-reader.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

sdb_data_points_reader_t *sdb_data_points_reader_create(int points_count) {
  sdb_data_points_reader_t *reader = (sdb_data_points_reader_t *)sdb_alloc(sizeof(sdb_data_points_reader_t));

  reader->points_count = points_count;
  reader->points = points_count == 0
                   ? NULL
                   : (data_point_t *)sdb_alloc(points_count * sizeof(data_point_t));
  reader->_write_position = 0;

  return reader;
}

int sdb_data_points_reader_write(sdb_data_points_reader_t *reader, data_point_t *points, int count) {
  if (count == 0 || reader->points == NULL) {
    return 0;
  }

  int to_write = sdb_min(count, reader->points_count - reader->_write_position);
  memcpy(reader->points + reader->_write_position, points, to_write * sizeof(data_point_t));
  reader->_write_position += to_write;

  return reader->_write_position < reader->points_count;
}

void sdb_data_points_reader_destroy(sdb_data_points_reader_t *reader) {
  sdb_free(reader->points);
  sdb_free(reader);
}
