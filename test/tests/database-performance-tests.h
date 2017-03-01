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

#ifndef TEST_TESTS_DATABASE_PERFORMANCE_TESTS_H_
#define TEST_TESTS_DATABASE_PERFORMANCE_TESTS_H_

#include "test/tests/base-database-tests.h"

namespace shakadb {
namespace test {

class DatabasePerformanceTests : public BaseDatabaseTests {
 public:
  Stopwatch sequential_write_small(TestContext ctx);
  Stopwatch sequential_write_medium(TestContext ctx);
  Stopwatch sequential_write_large(TestContext ctx);
  Stopwatch read_small(TestContext ctx);
  Stopwatch read_medium(TestContext ctx);
  Stopwatch read_large(TestContext ctx);
  Stopwatch random_write_small(TestContext ctx);
  Stopwatch random_write_medium(TestContext ctx);
  Stopwatch random_write_large(TestContext ctx);

 private:
  Stopwatch SequentialWrite(TestContext ctx, int batches, int batch_size);
  Stopwatch Read(TestContext ctx, int windows_count, int window_size);
  Stopwatch RandomWrite(TestContext ctx, int batches, int batch_size);
};

}  // namespace test
}  // namespace shakadb

#endif  // TEST_TESTS_DATABASE_PERFORMANCE_TESTS_H_
