//
// Created by Pawel Burzynski on 29/01/2017.
//

#ifndef APOLLO_STORAGE_CONCURRENCY_TESTS_H
#define APOLLO_STORAGE_CONCURRENCY_TESTS_H

#include <src/engine/database.h>
#include <memory>
#include <test/framework/assert.h>
#include <src/utils/file-log.h>
#include <src/utils/stopwatch.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "common.h"

namespace apollo {
namespace test {

void database_concurrent_access_writer(Database *db, int batches, int batch_size, int write_delay) {
  for (int i = 0; i < batches; i++) {
    write_to_database(db, "XOM_US", 1, batch_size, (timestamp_t)(i * batch_size));
    write_to_database(db, "MSFT_US", 1, batch_size, (timestamp_t)(i * batch_size));
    write_to_database(db, "APPL_US", 1, batch_size, (timestamp_t)(i * batch_size));
    write_to_database(db, "AAL_LN", 1, batch_size, (timestamp_t)(i * batch_size));
    std::this_thread::sleep_for(std::chrono::microseconds(write_delay));
  }
}

void database_concurrent_access_reader(Database *db, volatile bool *should_terminate) {

  while (!*should_terminate) {
    validate_read(db, "XOM_US", -1, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
    validate_read(db, "MSFT_US", -1, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
    validate_read(db, "APPL_US", -1, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
    validate_read(db, "AAL_LN", -1, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }
}

Stopwatch database_concurrent_access(TestContext ctx,
                                     int readers_count,
                                     int write_batches,
                                     int write_batch_size,
                                     int write_delay) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(10000, 100, ctx));
  std::list<std::thread> readers;
  Stopwatch sw;
  bool should_terminate = false;

  for (int i = 0; i < readers_count; i++) {
    readers.push_back(std::thread(database_concurrent_access_reader, c->GetDb(), &should_terminate));
  }

  sw.Start();
  std::thread writer(database_concurrent_access_writer, c->GetDb(), write_batches, write_batch_size, write_delay);
  writer.join();
  sw.Stop();

  return sw;
}

Stopwatch database_concurrent_access_small(TestContext ctx) {
  return database_concurrent_access(ctx, 5, 1, 100, 10);
}

}
}

#endif //APOLLO_STORAGE_CONCURRENCY_TESTS_H
