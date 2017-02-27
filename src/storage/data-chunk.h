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

#ifndef SHAKADB_STORAGE_DATACHUNK_H
#define SHAKADB_STORAGE_DATACHUNK_H

#include <string>
#include <src/data-point.h>
#include <src/utils/rw-lock.h>

namespace shakadb {

class DataChunk {
 public:
  ~DataChunk();
  static DataChunk *Load(std::string file_name, uint64_t file_offset, int max_points);
  static int CalculateChunkSize(int points);

  data_point_t *Read();
  void Write(int offset, data_point_t *points, int count);

  timestamp_t GetBegin();
  timestamp_t GetEnd();
  int GetNumberOfPoints();
  int GetMaxNumberOfPoints();

  void PrintMetadata();
 private:
  DataChunk(std::string file_name, uint64_t file_offset, int max_points);

  std::string file_name;
  uint64_t file_offset;
  int max_points;
  data_point_t *cached_content;
  RwLock lock;

  timestamp_t begin;
  timestamp_t end;
  int number_of_points;
};

}

#endif //SHAKADB_STORAGE_DATACHUNK_H
