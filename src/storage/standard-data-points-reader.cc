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

#include "src/storage/standard-data-points-reader.h"

#include <cstdlib>
#include <src/utils/memory.h>

#include "src/utils/allocator.h"

namespace shakadb {

StandardDataPointsReader::StandardDataPointsReader(int points_count) {
  this->points_count = points_count;
  this->points = (sdb_data_point_t *)sdb_alloc(points_count * sizeof(sdb_data_point_t));
  this->write_position = 0;
}

sdb_data_point_t *StandardDataPointsReader::GetDataPoints() {
  return this->points;
}

bool StandardDataPointsReader::WriteDataPoints(sdb_data_point_t *points, int count) {
  if (count == 0) {
    return false;
  }

  int to_write = std::min(count, this->GetDataPointsCount() - this->write_position);
  memcpy(this->GetDataPoints() + this->write_position, points, to_write * sizeof(data_point_t));
  this->write_position += to_write;

  return this->write_position < this->GetDataPointsCount();
}

int StandardDataPointsReader::GetDataPointsCount() {
  return this->points_count;
}

}  // namespace shakadb
