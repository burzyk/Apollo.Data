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

#include "test/tests/database-performance-tests.h"

#include <memory>

#include "src/utils/stopwatch.h"
#include "test/framework/test-context.h"

namespace shakadb {
namespace test {

Stopwatch DatabasePerformanceTests::sequential_write_small(TestContext ctx) {
  return this->SequentialWrite(ctx, 1000, 100);
}

Stopwatch DatabasePerformanceTests::sequential_write_medium(TestContext ctx) {
  return this->SequentialWrite(ctx, 10000, 100);
}

Stopwatch DatabasePerformanceTests::sequential_write_large(TestContext ctx) {
  return this->SequentialWrite(ctx, 100000, 100);
}

Stopwatch DatabasePerformanceTests::read_small(TestContext ctx) {
  return this->Read(ctx, 1000, 100);
}

Stopwatch DatabasePerformanceTests::read_medium(TestContext ctx) {
  return this->Read(ctx, 10000, 100);
}

Stopwatch DatabasePerformanceTests::read_large(TestContext ctx) {
  return this->Read(ctx, 100000, 100);
}

Stopwatch DatabasePerformanceTests::random_write_small(TestContext ctx) {
  return this->RandomWrite(ctx, 100, 100);
}

Stopwatch DatabasePerformanceTests::random_write_medium(TestContext ctx) {
  return this->RandomWrite(ctx, 1000, 100);
}

Stopwatch DatabasePerformanceTests::random_write_large(TestContext ctx) {
  return this->RandomWrite(ctx, 10000, 100);
}

Stopwatch DatabasePerformanceTests::SequentialWrite(TestContext ctx, int batches, int batch_size) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(10000, 100, ctx));
  Stopwatch sw;

  sw.Start();
  Write(db.get(), "usd_gbp", batches, batch_size);
  sw.Stop();

  return sw;
}

Stopwatch DatabasePerformanceTests::Read(TestContext ctx, int windows_count, int window_size) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(10000, 100, ctx));
  Stopwatch sw;

  Write(db.get(), "usd_gbp", windows_count, window_size);

  sw.Start();
  for (int i = 0; i < windows_count; i++) {
    ValidateRead(db.get(),
                 "usd_gbp",
                 i == 0 ? window_size - 1 : window_size,
                 (timestamp_t)(i * window_size),
                 (timestamp_t)((i + 1) * window_size));
  }
  sw.Stop();

  return sw;
}

Stopwatch DatabasePerformanceTests::RandomWrite(TestContext ctx, int batches, int batch_size) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(10000, 100, ctx));

  // for random but consistent results
  srand(0);
  Stopwatch sw;

  sw.Start();

  for (int i = 0; i < batches; i++) {
    unsigned int r;
    rand_r(&r);
    int time = r % batch_size + 1;
    Write(db.get(), "usd_gbp", 1, batch_size, (timestamp_t)time);
  }

  sw.Stop();

  ValidateRead(db.get(),
               "usd_gbp",
               batch_size * batches,
               data_point_t::kMinTimestamp,
               data_point_t::kMaxTimestamp);

  return sw;
}

}  // namespace test
}  // namespace shakadb
