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

#ifndef SRC_STORAGE_STANDARD_DATA_POINTS_READER_H_
#define SRC_STORAGE_STANDARD_DATA_POINTS_READER_H_

#include <list>
#include <vector>

#include "src/data-point.h"
#include "src/storage/data-chunk.h"
#include "src/storage/data-points-reader.h"

namespace shakadb {

class StandardDataPointsReader : public DataPointsReader {
 public:
  explicit StandardDataPointsReader(int points_count);

  bool WriteDataPoints(data_point_t *points, int count);
  data_point_t *GetDataPoints();
  int GetDataPointsCount();

 private:
  data_point_t *points;
  int points_count;
  int write_position;
};

}  // namespace shakadb

#endif  // SRC_STORAGE_STANDARD_DATA_POINTS_READER_H_
