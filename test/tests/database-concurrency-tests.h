//
// Created by Pawel Burzynski on 29/01/2017.
//

#ifndef SHAKADB_STORAGE_CONCURRENCY_TESTS_H
#define SHAKADB_STORAGE_CONCURRENCY_TESTS_H

#include <src/storage/database.h>
#include <memory>
#include <test/framework/assert.h>
#include <src/file-log.h>
#include <src/utils/stopwatch.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <src/fatal-exception.h>
#include "common.h"

namespace shakadb {
namespace test {

class DatabaseConcurrencyTests : public BaseDatabaseTest {
 public:
  DatabaseConcurrencyTests(DatabaseContextFactory *context_factory)
      : BaseDatabaseTest(context_factory) {
  }

  Stopwatch access_small(TestContext ctx) {
    return this->ConcurrentAccess(ctx, 5, 100, 100);
  };

  Stopwatch access_medium(TestContext ctx) {
    return this->ConcurrentAccess(ctx, 5, 200, 100);
  };

  Stopwatch access_large(TestContext ctx) {
    return this->ConcurrentAccess(ctx, 5, 300, 100);
  };
 private:
  static void ConcurrentAccessWriter(Database *db, int batches, int batch_size) {
    try {
      WriteToDatabase(db, "XOM_US", batches, batch_size);
      WriteToDatabase(db, "MSFT_US", batches, batch_size);
      WriteToDatabase(db, "APPL_US", batches, batch_size);
      WriteToDatabase(db, "AAL_LN", batches, batch_size);
    } catch (FatalException ex) {
      printf("Writer fatal exception: %s\n", ex.what());
    }
  };

  static void ConcurrentAccessReader(Database *db, volatile bool *should_terminate) {
    try {
      while (!*should_terminate) {
        validate_read(db, "XOM_US", -1, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
        validate_read(db, "MSFT_US", -1, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
        validate_read(db, "APPL_US", -1, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
        validate_read(db, "AAL_LN", -1, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
      }
    } catch (FatalException ex) {
      printf("Reader fatal exception: %s\n", ex.what());
    }
  };

  Stopwatch ConcurrentAccess(TestContext ctx, int readers_count, int write_batches, int write_batch_size) {
    auto c = std::unique_ptr<DatabaseContext>(this->CreateContext(10000, 100, ctx));
    std::list<std::thread *> readers;
    Stopwatch sw;
    bool should_terminate = false;

    for (int i = 0; i < readers_count; i++) {
      readers.push_back(new std::thread(ConcurrentAccessReader, c->GetDb(), &should_terminate));
    }

    sw.Start();
    std::thread writer(ConcurrentAccessWriter, c->GetDb(), write_batches, write_batch_size);
    writer.join();
    sw.Stop();

    should_terminate = true;

    for (auto reader: readers) {
      reader->join();
      delete reader;
    }

    return sw;
  };
};

}
}

#endif //SHAKADB_STORAGE_CONCURRENCY_TESTS_H
