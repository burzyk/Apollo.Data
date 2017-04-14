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

#ifndef SRC_STORAGE_STANDARD_DATABASE_H_
#define SRC_STORAGE_STANDARD_DATABASE_H_

#include <string>
#include <list>
#include <map>

#include "src/storage/data-chunk.h"
#include "src/storage/data-series.h"
#include "src/storage/database.h"

namespace shakadb {

class StandardDatabase : public Database {
 public:
  ~StandardDatabase();
  static StandardDatabase *Init(std::string directory, int points_per_chunk);

  sdb_data_points_reader_t *Read(sdb_data_series_id_t series_id,
                                 sdb_timestamp_t begin,
                                 sdb_timestamp_t end,
                                 int max_points);
  int Write(sdb_data_series_id_t series_id, sdb_data_point_t *points, int count);
  void Truncate(sdb_data_series_id_t series_id);

 private:
  StandardDatabase(std::string directory, int points_per_chunk);
  DataSeries *FindDataSeries(sdb_data_series_id_t series_id);

  std::string directory;
  int points_per_chunk;
  sdb_rwlock_t *lock;
  std::map<sdb_data_series_id_t, DataSeries *> series;
};

}  // namespace shakadb

#endif  // SRC_STORAGE_STANDARD_DATABASE_H_
