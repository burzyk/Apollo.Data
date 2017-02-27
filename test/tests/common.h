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
// Created by Pawel Burzynski on 29/01/2017.
//

#ifndef SHAKADB_STORAGE_STORAGE_COMMON_H
#define SHAKADB_STORAGE_STORAGE_COMMON_H

#include <src/storage/database.h>
#include <memory>
#include <limits.h>
#include <test/framework/assert.h>
#include <src/file-log.h>
#include <src/utils/stopwatch.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <src/fatal-exception.h>
#include <test/common.h>
#include <src/storage/standard-database.h>

namespace shakadb {
namespace test {

class DatabaseContext {
 public:
  virtual ~DatabaseContext() {};
  virtual Database *GetDb() = 0;
};

class DatabaseContextFactory {
 public:
  virtual DatabaseContext *Create(int points_per_chunk, int max_pages, TestContext ctx) = 0;
};

class StandardDatabaseContextFactory : public DatabaseContextFactory {
 public:
  DatabaseContext *Create(int points_per_chunk, int max_pages, TestContext ctx) {
    Log *log = new NullLog();
    Database *db = StandardDatabase::Init(
        ctx.GetWorkingDirectory(),
        log,
        points_per_chunk,
        max_pages * points_per_chunk * sizeof(data_point_t));
    return new StandardDatabaseContext(log, db);
  }
 private:
  class StandardDatabaseContext : public DatabaseContext {
   public:
    StandardDatabaseContext(Log *log, Database *db) {
      this->log = log;
      this->db = db;
    };

    ~StandardDatabaseContext() {
      delete this->db;
      delete this->log;
    }

    Database *GetDb() {
      return this->db;
    }
   private:
    Log *log;
    Database *db;
  };
};

class BaseDatabaseTest {
 public:
  BaseDatabaseTest(DatabaseContextFactory *context_factory) {
    this->context_factory = context_factory;
  };

 protected:
  DatabaseContext *CreateContext(int points_per_chunk, int max_pages, TestContext ctx) {
    return this->context_factory->Create(points_per_chunk, max_pages, ctx);
  };

  static void WriteToDatabase(Database *db,
                              std::string series_name,
                              int batches,
                              int batch_size,
                              timestamp_t time = 1) {
    if (time == 0) {
      throw FatalException("Time cannot be 0");
    }

    shakadb::data_point_t *points = Allocator::New<shakadb::data_point_t>(batch_size);

    for (int i = 0; i < batches; i++) {
      for (int j = 0; j < batch_size; j++) {
        points[j].time = time;
        points[j].value = time * 100;
        time++;
      }

      db->Write(series_name, points, batch_size);
    }

    shakadb::Allocator::Delete(points);
  };

  static void ValidateRead(Database *db,
                           std::string series_name,
                           int expected_count,
                           timestamp_t begin,
                           timestamp_t end,
                           int max_points = INT_MAX) {
    auto reader = std::unique_ptr<DataPointsReader>(db->Read(series_name, begin, end, max_points));
    int total_read = reader->GetDataPointsCount();
    data_point_t *points = reader->GetDataPoints();

    for (int i = 1; i < total_read; i++) {
      Assert::IsTrue(points[i - 1].time <= points[i].time);
      Assert::IsTrue(points[i].time != 0);
    }

    if (expected_count > 0) {
      Assert::IsTrue(expected_count == total_read);
    }
  };
 private:
  DatabaseContextFactory *context_factory;
};

}
}

#endif //SHAKADB_STORAGE_COMMON_H
