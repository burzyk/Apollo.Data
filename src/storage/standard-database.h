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

#ifndef SHAKADB_STORAGE_STANDARDDATABASE_H
#define SHAKADB_STORAGE_STANDARDDATABASE_H

#include <string>
#include <list>
#include <map>
#include <src/utils/rw-lock.h>
#include <src/log.h>
#include "data-points-reader.h"
#include "data-chunk.h"
#include "data-series.h"
#include "database.h"

namespace shakadb {

class StandardDatabase : public Database {
 public:
  ~StandardDatabase();
  static StandardDatabase *Init(std::string directory, Log *log, int points_per_chunk, int cache_memory_limit);

  DataPointsReader *Read(std::string name, timestamp_t begin, timestamp_t end, int max_points);
  void Write(std::string name, data_point_t *points, int count);
 private:
  StandardDatabase(std::string directory, Log *log, int points_per_chunk, int cache_memory_limit);
  DataSeries *FindDataSeries(std::string name);

  std::string directory;
  int points_per_chunk;
  int cache_memory_limit;
  RwLock lock;
  Log *log;
  std::map<std::string, DataSeries *> series;
};

}

#endif //SHAKADB_STORAGE_STANDARDDATABASE_H
