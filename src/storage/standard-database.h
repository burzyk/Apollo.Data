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
#include "src/data-points-reader.h"
#include "data-chunk.h"
#include "data-series.h"
#include "src/database.h"

namespace shakadb {

class StandardDatabase : public Database {
 public:
  ~StandardDatabase();
  static StandardDatabase *Init(std::string directory, Log *log, int points_per_chunk, int cache_memory_limit);

  std::shared_ptr<DataPointsReader> Read(std::string name, timestamp_t begin, timestamp_t end);
  void Write(std::string name, data_point_t *points, int count);

  void PrintMetadata();
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
