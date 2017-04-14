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

#ifndef TEST_TESTS_BASE_DATABASE_TESTS_H_
#define TEST_TESTS_BASE_DATABASE_TESTS_H_

#include <climits>
#include <string>

#include "src/storage/database.h"
#include "src/file-log.h"
#include "src/fatal-exception.h"
#include "src/storage/standard-database.h"
#include "src/utils/allocator.h"
#include "test/framework/test-context.h"
#include "test/framework/assert.h"
#include "test/tests/null-log.h"

namespace shakadb {
namespace test {

class BaseDatabaseTests {
 public:
  virtual ~BaseDatabaseTests() {}
 protected:
  static void Write(Database *db, data_series_id_t series_id, int batches, int count, timestamp_t time = 1);
  static void ValidateRead(Database *db,
                           data_series_id_t series_id,
                           int expected_count,
                           timestamp_t begin,
                           timestamp_t end,
                           int max_points = INT_MAX);

  Database *CreateDatabase(int points_per_chunk, int max_pages, TestContext ctx);
  Log *GetLog();

 private:
  NullLog log;
};

}  // namespace test
}  // namespace shakadb

#endif  // TEST_TESTS_BASE_DATABASE_TESTS_H_
