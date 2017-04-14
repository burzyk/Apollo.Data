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

#include <list>
#include <string>

#include "src/storage/data-points-reader.h"
#include "src/storage/data-chunk.h"

namespace shakadb {

class DataSeries {
 public:
  ~DataSeries();
  static DataSeries *Init(std::string file_name, int points_per_chunk);

  DataPointsReader *Read(timestamp_t begin, timestamp_t end, int max_points);
  void Write(sdb_data_point_t *points, int count);
  void Truncate();

 private:
  DataSeries(std::string file_name, int points_per_chunk);
  void RegisterChunk(sdb_data_chunk_t *chunk);
  void WriteChunk(sdb_data_chunk_t *chunk, sdb_data_point_t *points, int count);
  void ChunkMemcpy(sdb_data_chunk_t *chunk, int position, sdb_data_point_t *points, int count);
  sdb_data_chunk_t *CreateEmptyChunk();
  void DeleteChunks();

  std::string file_name;
  int points_per_chunk;
  std::list<sdb_data_chunk_t *> chunks;
  sdb_rwlock_t *series_lock;
};

}  // namespace shakadb

#endif  // SRC_STORAGE_DATA_SERIES_H_
