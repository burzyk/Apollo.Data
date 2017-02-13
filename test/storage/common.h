//
// Created by Pawel Burzynski on 29/01/2017.
//

#ifndef SHAKADB_STORAGE_STORAGE_COMMON_H
#define SHAKADB_STORAGE_STORAGE_COMMON_H

#include <src/storage/database.h>
#include <memory>
#include <test/framework/assert.h>
#include <src/utils/file-log.h>
#include <src/utils/stopwatch.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <src/fatal-exception.h>
#include <test/common.h>

namespace shakadb {
namespace test {

class DatabaseContext {
 public:
  static DatabaseContext *Create(int points_per_chunk, int max_pages, TestContext ctx) {
    DatabaseContext *context = new DatabaseContext();
    context->log = new NullLog();
    context->db = Database::Init(
        ctx.GetWorkingDirectory(),
        context->log,
        points_per_chunk,
        max_pages * points_per_chunk * sizeof(data_point_t));
    return context;
  }

  ~DatabaseContext() {
    delete this->db;
  }

  Database *GetDb() {
    return this->db;
  }
 private:
  DatabaseContext() {}

  Log *log;
  Database *db;
};

void write_to_database(Database *db, std::string series_name, int batches, int batch_size, timestamp_t time) {
  if (time == 0) {
    throw FatalException("Time cannot be 0");
  }

  shakadb::data_point_t *points = new shakadb::data_point_t[batch_size];

  for (int i = 0; i < batches; i++) {
    for (int j = 0; j < batch_size; j++) {
      points[j].time = time;
      points[j].value = time * 100;
      time++;
    }

    db->Write(series_name, points, batch_size);
  }

  delete[] points;
}

void write_to_database(Database *db, std::string series_name, int batches, int batch_size) {
  write_to_database(db, series_name, batches, batch_size, 1);
}

void validate_read(Database *db, std::string series_name, int expected_count, timestamp_t begin, timestamp_t end) {
  std::shared_ptr<DataPointReader> reader = db->Read(series_name, begin, end);
  int total_read = reader->GetDataPointsCount();
  data_point_t *points = reader->GetDataPoints();

  for (int i = 1; i < total_read; i++) {
    Assert::IsTrue(points[i - 1].time <= points[i].time);
    Assert::IsTrue(points[i].time != 0);
  }

  if (expected_count > 0) {
    Assert::IsTrue(expected_count == total_read);
  }
}

}
}

#endif //SHAKADB_STORAGE_COMMON_H
