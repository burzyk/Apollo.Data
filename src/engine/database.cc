//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cstdlib>
#include <cmath>
#include <src/utils/common.h>
#include <src/utils/stopwatch.h>
#include <src/utils/directory.h>
#include "database.h"

namespace apollo {

Database::Database(std::string directory, Log *log, int points_per_chunk, int cache_memory_limit) {
  this->directory = directory;
  this->log = log;
  this->points_per_chunk = points_per_chunk;
  this->cache_memory_limit = cache_memory_limit;
}

Database::~Database() {
  this->log->Info("Deleting database");

  for (auto s: this->series) {
    delete s.second;
  }

  this->series.clear();
}

Database *Database::Init(std::string directory, Log *log, int points_per_chunk, int cache_memory_limit) {
  return new Database(directory, log, points_per_chunk, cache_memory_limit);
}

void Database::Write(std::string name, data_point_t *points, int count) {
  DataSeries *series = this->FindDataSeries(name);
  series->Write(points, count);
}

DataPointReader Database::Read(std::string name, timestamp_t begin, timestamp_t end) {
  DataSeries *series = this->FindDataSeries(name);
  return series->Read(begin, end);
}

void Database::PrintMetadata() {
  printf("Database:\n");

  for (auto series: this->series) {
    printf("==================================================\n");
    printf("Series: %s\n", series.first.c_str());

    series.second->PrintMetadata();
  }
}

DataSeries *Database::FindDataSeries(std::string name) {
  auto scope = std::unique_ptr<RwLockScope>(this->lock.LockRead());

  if (this->series.find(name) == this->series.end()) {
    scope->UpgradeToWrite();

    if (this->series.find(name) == this->series.end()) {
      this->series[name] = DataSeries::Init(this->directory + "/" + name, this->points_per_chunk, this->log);
    }
  }

  return this->series[name];
}

}