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

#include <cstdlib>
#include <cmath>
#include <src/utils/directory.h>
#include "standard-database.h"

namespace shakadb {

StandardDatabase::StandardDatabase(std::string directory, Log *log, int points_per_chunk, int cache_memory_limit) {
  this->directory = directory;
  this->log = log;
  this->points_per_chunk = points_per_chunk;
  this->cache_memory_limit = cache_memory_limit;
}

StandardDatabase::~StandardDatabase() {
  for (auto s: this->series) {
    delete s.second;
  }

  this->series.clear();
}

StandardDatabase *StandardDatabase::Init(std::string directory, Log *log, int points_per_chunk, int cache_memory_limit) {
  return new StandardDatabase(directory, log, points_per_chunk, cache_memory_limit);
}

void StandardDatabase::Write(std::string name, data_point_t *points, int count) {
  DataSeries *series = this->FindDataSeries(name);
  series->Write(points, count);
}

DataPointsReader *StandardDatabase::Read(std::string name, timestamp_t begin, timestamp_t end, int max_points) {
  DataSeries *series = this->FindDataSeries(name);
  return series->Read(begin, end, max_points);
}

void StandardDatabase::PrintMetadata() {
  printf("StandardDatabase:\n");

  for (auto series: this->series) {
    printf("==================================================\n");
    printf("Series: %s\n", series.first.c_str());

    series.second->PrintMetadata();
  }
}

DataSeries *StandardDatabase::FindDataSeries(std::string name) {
  auto scope = this->lock.LockRead();

  if (this->series.find(name) == this->series.end()) {
    scope->UpgradeToWrite();

    if (this->series.find(name) == this->series.end()) {
      this->series[name] = DataSeries::Init(this->directory + "/" + name, this->points_per_chunk, this->log);
    }
  }

  return this->series[name];
}

}