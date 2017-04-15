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
// Created by Pawel Burzynski on 01/03/2017.
//

#include "test/tests/base-database-tests.h"

#include <memory>

#include "src/utils/allocator.h"
#include "src/fatal-exception.h"

namespace shakadb {
namespace test {

void BaseDatabaseTests::Write(Database *db, data_series_id_t series_id, int batches, int count, timestamp_t time) {
  if (time == 0) {
    throw FatalException("Time cannot be 0");
  }

  shakadb::data_point_t *points = Allocator::New<shakadb::data_point_t>(count);

  for (int i = 0; i < batches; i++) {
    for (int j = 0; j < count; j++) {
      points[j].time = time;
      points[j].value = time * 100;
      time++;
    }

    db->Write(series_id, points, count);
  }

  shakadb::Allocator::Delete(points);
}

void BaseDatabaseTests::ValidateRead(Database *db,
                                     data_series_id_t series_id,
                                     int expected_count,
                                     timestamp_t begin,
                                     timestamp_t end,
                                     int max_points) {
  auto reader = std::unique_ptr<DataPointsReader>(db->Read(series_id, begin, end, max_points));
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

Database *BaseDatabaseTests::CreateDatabase(int points_per_chunk, int max_pages, TestContext ctx) {
  return StandardDatabase::Init(ctx.GetWorkingDirectory(), this->GetLog(), points_per_chunk);
}

Log *BaseDatabaseTests::GetLog() {
  return &this->log;
}

}  // namespace test
}  // namespace shakadb
