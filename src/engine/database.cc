//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cstdlib>
#include <cmath>
#include <src/utils/common.h>
#include <src/utils/stopwatch.h>
#include <src/engine/storage/cached-storage.h>
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
  Stopwatch sw;
  Database *db = new Database(directory, log, points_per_chunk, cache_memory_limit);

  log->Info("Initializing database: " + directory);
  sw.Start();

  for (auto file: Directory::GetFiles(directory)) {
    log->Info("Loading data series: " + file);
    db->series[file] = DataSeries::Init(db->InitStorage(file), log);
  }

  sw.Stop();

  log->Info("Database loaded in: " + std::to_string(sw.GetElapsedMilliseconds() / 1000) + "[s]");
  return db;
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
  if (this->series.find(name) == this->series.end()) {
    this->series[name] = DataSeries::Init(this->InitStorage(name), this->log);
  }

  return this->series[name];
}

Storage *Database::InitStorage(std::string series_name) {
  int page_size = this->points_per_chunk * sizeof(apollo::data_point_t);
  int max_pages = this->cache_memory_limit / page_size;

  return CachedStorage::Init(this->directory + "/" + series_name, page_size, max_pages);
}

}