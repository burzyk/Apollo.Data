//
// Created by Pawel Burzynski on 29/01/2017.
//

#ifndef SHAKADB_STORAGE_CONCURRENCY_TESTS_H
#define SHAKADB_STORAGE_CONCURRENCY_TESTS_H

#include <src/database.h>
#include <memory>
#include <test/framework/assert.h>
#include <src/file-log.h>
#include <src/utils/stopwatch.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <src/fatal-exception.h>
#include "base-database-tests.h"

namespace shakadb {
namespace test {

class DatabaseConcurrencyTests : public BaseDatabaseTest {
 public:
  DatabaseConcurrencyTests(DatabaseContextFactory *context_factory)
      : BaseDatabaseTest(context_factory) {
  }

  Stopwatch access_small(TestContext ctx) {
    return this->concurrent_access(ctx, 5, 100, 100);
  };

  Stopwatch access_medium(TestContext ctx) {
    return this->concurrent_access(ctx, 5, 200, 100);
  };

  Stopwatch access_large(TestContext ctx) {
    return this->concurrent_access(ctx, 5, 300, 100);
  };
 private:
  static void concurrent_access_writer(Database *db, int batches, int batch_size) {
    try {
      write_to_database(db, "XOM_US", batches, batch_size);
      write_to_database(db, "MSFT_US", batches, batch_size);
      write_to_database(db, "APPL_US", batches, batch_size);
      write_to_database(db, "AAL_LN", batches, batch_size);
    } catch (FatalException ex) {
      printf("Writer fatal exception: %s\n", ex.what());
    }
  };

  static void concurrent_access_reader(Database *db, volatile bool *should_terminate) {
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

  Stopwatch concurrent_access(TestContext ctx, int readers_count, int write_batches, int write_batch_size) {
    auto c = std::unique_ptr<DatabaseContext>(this->CreateContext(10000, 100, ctx));
    std::list<std::thread *> readers;
    Stopwatch sw;
    bool should_terminate = false;

    for (int i = 0; i < readers_count; i++) {
      readers.push_back(new std::thread(concurrent_access_reader, c->GetDb(), &should_terminate));
    }

    sw.Start();
    std::thread writer(concurrent_access_writer, c->GetDb(), write_batches, write_batch_size);
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
