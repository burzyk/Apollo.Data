//
// Created by Pawel Burzynski on 29/01/2017.
//

#ifndef SHAKADB_STORAGE_PERFORMANCE_TESTS_H
#define SHAKADB_STORAGE_PERFORMANCE_TESTS_H

#include <src/database.h>
#include <memory>
#include <test/framework/assert.h>
#include <src/file-log.h>
#include <src/utils/stopwatch.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "database-common.h"

namespace shakadb {
namespace test {

class DatabasePerformanceTests {
 public:
  Stopwatch sequential_write_small(TestContext ctx) {
    return this->sequential_write(ctx, 1000, 100);
  };

  Stopwatch sequential_write_medium(TestContext ctx) {
    return this->sequential_write(ctx, 10000, 100);
  };

  Stopwatch sequential_write_large(TestContext ctx) {
    return this->sequential_write(ctx, 100000, 100);
  };

  Stopwatch read_small(TestContext ctx) {
    return this->read(ctx, 1000, 100);
  };

  Stopwatch read_medium(TestContext ctx) {
    return this->read(ctx, 10000, 100);
  };

  Stopwatch read_large(TestContext ctx) {
    return this->read(ctx, 100000, 100);
  };

  Stopwatch random_write_small(TestContext ctx) {
    return this->random_write(ctx, 100, 100);
  };

  Stopwatch random_write_medium(TestContext ctx) {
    return this->random_write(ctx, 1000, 100);
  };

  Stopwatch random_write_large(TestContext ctx) {
    return this->random_write(ctx, 10000, 100);
  };

 private:
  Stopwatch sequential_write(TestContext ctx, int batches, int batch_size) {
    auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(10000, 100, ctx));
    Stopwatch sw;

    sw.Start();
    write_to_database(c->GetDb(), "usd_gbp", batches, batch_size);
    sw.Stop();

    return sw;
  };

  Stopwatch read(TestContext ctx, int windows_count, int window_size) {
    auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(10000, 100, ctx));
    Stopwatch sw;

    write_to_database(c->GetDb(), "usd_gbp", windows_count, window_size);

    sw.Start();
    for (int i = 0; i < windows_count; i++) {
      validate_read(c->GetDb(),
                    "usd_gbp",
                    i == 0 ? window_size - 1 : window_size,
                    (timestamp_t)(i * window_size),
                    (timestamp_t)((i + 1) * window_size));
    }
    sw.Stop();

    return sw;
  };

  Stopwatch random_write(TestContext ctx, int batches, int batch_size) {
    auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(10000, 100, ctx));

    // for random but consistent results
    srand(0);
    Stopwatch sw;

    sw.Start();

    for (int i = 0; i < batches; i++) {
      int time = rand() % batch_size + 1;
      write_to_database(c->GetDb(), "usd_gbp", 1, batch_size, (timestamp_t)time);
    }

    sw.Stop();

    validate_read(c->GetDb(),
                  "usd_gbp",
                  batch_size * batches,
                  data_point_t::kMinTimestamp,
                  data_point_t::kMaxTimestamp);

    return sw;
  };

};

}
}

#endif //SHAKADB_STORAGE_PERFORMANCE_TESTS_H
