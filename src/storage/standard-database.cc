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

#include "src/storage/standard-database.h"

#include <cstdlib>
#include <cmath>

namespace shakadb {

StandardDatabase::StandardDatabase(std::string directory, int points_per_chunk) {
  this->directory = directory;
  this->points_per_chunk = points_per_chunk;
  this->lock = sdb_rwlock_create();
}

StandardDatabase::~StandardDatabase() {
  for (auto s : this->series) {
    delete s.second;
  }

  this->series.clear();
  sdb_rwlock_destroy(this->lock);
}

StandardDatabase *StandardDatabase::Init(std::string directory, int points_per_chunk) {
  return new StandardDatabase(directory, points_per_chunk);
}

int StandardDatabase::Write(sdb_data_series_id_t series_id, sdb_data_point_t *points, int count) {
  DataSeries *series = this->FindDataSeries(series_id);
  series->Write(points, count);

  return 0;
}

void StandardDatabase::Truncate(sdb_data_series_id_t series_id) {
  DataSeries *series = this->FindDataSeries(series_id);
  series->Truncate();
}

sdb_data_points_reader_t *StandardDatabase::Read(sdb_data_series_id_t series_id,
                                                 sdb_timestamp_t begin,
                                                 sdb_timestamp_t end,
                                                 int max_points) {
  DataSeries *series = this->FindDataSeries(series_id);
  return series->Read(begin, end, max_points);
}

DataSeries *StandardDatabase::FindDataSeries(sdb_data_series_id_t series_id) {
  sdb_rwlock_rdlock(this->lock);

  if (this->series.find(series_id) == this->series.end()) {
    sdb_rwlock_upgrade(this->lock);

    if (this->series.find(series_id) == this->series.end()) {
      this->series[series_id] =
          DataSeries::Init(this->directory + "/" + std::to_string(series_id), this->points_per_chunk);
    }
  }

  DataSeries *result = this->series[series_id];
  sdb_rwlock_unlock(this->lock);

  return result;
}

}  // namespace shakadb
