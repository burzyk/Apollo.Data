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

#include "test/tests/database-concurrency-tests.h"

#include <memory>
#include <list>

#include "src/utils/thread.h"
#include "src/utils/stopwatch.h"
#include "src/fatal-exception.h"
#include "src/storage/database.h"
#include "test/framework/test-context.h"

namespace shakadb {
namespace test {

Stopwatch DatabaseConcurrencyTests::access_small(TestContext ctx) {
  return this->ConcurrentAccess(ctx, 5, 100, 100);
}

Stopwatch DatabaseConcurrencyTests::access_medium(TestContext ctx) {
  return this->ConcurrentAccess(ctx, 5, 200, 100);
}

Stopwatch DatabaseConcurrencyTests::access_large(TestContext ctx) {
  return this->ConcurrentAccess(ctx, 5, 300, 100);
}

void DatabaseConcurrencyTests::ConcurrentAccessWriter(Database *db, int batches, int batch_size) {
  try {
    Write(db, "XOM_US", batches, batch_size);
    Write(db, "MSFT_US", batches, batch_size);
    Write(db, "APPL_US", batches, batch_size);
    Write(db, "AAL_LN", batches, batch_size);
  } catch (FatalException ex) {
    printf("Writer fatal exception: %s\n", ex.what());
  }
}

void DatabaseConcurrencyTests::ConcurrentAccessReader(Database *db, volatile bool *should_terminate) {
  try {
    while (!*should_terminate) {
      ValidateRead(db, "XOM_US", -1, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
      ValidateRead(db, "MSFT_US", -1, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
      ValidateRead(db, "APPL_US", -1, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
      ValidateRead(db, "AAL_LN", -1, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
    }
  } catch (FatalException ex) {
    printf("Reader fatal exception: %s\n", ex.what());
  }
}

Stopwatch DatabaseConcurrencyTests::ConcurrentAccess(TestContext ctx, int readers_count, int batches, int batch_size) {
  Database *db = this->CreateDatabase(10000, 100, ctx);
  std::list<Thread *> readers;
  Stopwatch sw;
  bool should_terminate = false;
  bool *p_should_terminate = &should_terminate;

  for (int i = 0; i < readers_count; i++) {
    readers.push_back(new Thread(
        [db, p_should_terminate](void *data) -> void { ConcurrentAccessReader(db, p_should_terminate); },
        this->GetLog()));
  }

  sw.Start();
  Thread writer(
      [db, batches, batch_size](void *data) -> void { ConcurrentAccessWriter(db, batches, batch_size); },
      this->GetLog());
  writer.Join();
  sw.Stop();

  should_terminate = true;

  for (auto reader : readers) {
    reader->Join();
    delete reader;
  }

  delete db;
  return sw;
}

}  // namespace test
}  // namespace shakadb
