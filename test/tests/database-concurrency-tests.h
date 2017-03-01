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

#ifndef TEST_TESTS_DATABASE_CONCURRENCY_TESTS_H_
#define TEST_TESTS_DATABASE_CONCURRENCY_TESTS_H_

#include "test/tests/base-database-tests.h"

namespace shakadb {
namespace test {

class DatabaseConcurrencyTests : public BaseDatabaseTests {
 public:
  Stopwatch access_small(TestContext ctx);
  Stopwatch access_medium(TestContext ctx);
  Stopwatch access_large(TestContext ctx);

 private:
  static void ConcurrentAccessWriter(Database *db, int batches, int batch_size);
  static void ConcurrentAccessReader(Database *db, volatile bool *should_terminate);
  Stopwatch ConcurrentAccess(TestContext ctx, int readers_count, int batches, int batch_size);
};

}  // namespace test
}  // namespace shakadb

#endif  // TEST_TESTS_DATABASE_CONCURRENCY_TESTS_H_
