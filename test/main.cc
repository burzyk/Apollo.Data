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
#include <cstdio>
#include <functional>

#include "test/framework/test-context.h"
#include "test/framework/test-runner.h"
#include "src/utils/directory.h"
#include "test/tests/ring-buffer-tests.h"
#include "src/utils/allocator.h"
#include "test/tests/monitor-tests.h"
#include "test/tests/base-database-tests.h"
#include "test/tests/database-basic-tests.h"
#include "test/tests/database-performance-tests.h"
#include "test/tests/database-concurrency-tests.h"
#include "test/tests/rw-lock-tests.h"
#include "test/tests/configuration-tests.h"
#include "test/tests/end-to-end.h"

#define RUN_TESTS
#define RUN_PERF_TESTS

#ifdef RUN_TESTS
#define TEST(clazz, test_case) result |= runner.RunTest(\
      "" #clazz "_" #test_case "", \
      [&clazz](shakadb::test::TestContext ctx) -> void { clazz.test_case(ctx); });
#else
#define TEST(clazz, test_case)
#endif

#ifdef RUN_PERF_TESTS
#define TEST_PERF(clazz, test_case) result |= runner.RunPerfTest(\
      "" #clazz "_" #test_case "", \
      [&clazz](shakadb::test::TestContext ctx) -> shakadb::Stopwatch { return clazz.test_case(ctx); });
#else
#define TEST_PERF(clazz, test_case)
#endif

int main() {
  std::string dir("/Users/pburzynski/shakadb-test/data/test-stuff");
  shakadb::Directory::CreateDirectory(dir);
  shakadb::test::TestRunner runner(dir);
  int result = 0;

  printf("==================== Running tests ====================\n");

  auto database_basic = shakadb::test::DatabaseBasicTests();
  TEST(database_basic, simple_database_initialization_test);
  TEST(database_basic, basic_database_write_and_read_all);
  TEST(database_basic, write_database_in_one_big_batch);
  TEST(database_basic, write_database_in_multiple_small_batches);
  TEST(database_basic, database_write_history);
  TEST(database_basic, database_write_close_and_write_more);
  TEST(database_basic, database_multi_write_and_read_all);
  TEST(database_basic, database_continuous_write);
  TEST(database_basic, database_continuous_write_with_pickup);
  TEST(database_basic, database_write_batch_size_equal_to_page_capacity);
  TEST(database_basic, database_write_batch_size_greater_than_page_capacity);
  TEST(database_basic, database_read_inside_single_chunk);
  TEST(database_basic, database_read_span_two_chunks);
  TEST(database_basic, database_read_span_three_chunks);
  TEST(database_basic, database_read_chunk_edges);
  TEST(database_basic, database_read_duplicated_values);
  TEST(database_basic, database_read_with_limit);

//  auto end_to_end = shakadb::test::EndToEnd();
//  TEST(end_to_end, empty_read);
//  TEST(end_to_end, write_multiple);
//  TEST(end_to_end, write_small);
//  TEST(end_to_end, write_stop_read);

  auto ring_buffer = shakadb::test::RingBufferTests();
  TEST(ring_buffer, create_delete_test);
  TEST(ring_buffer, empty_read_test);
  TEST(ring_buffer, empty_peek_test);
  TEST(ring_buffer, simple_write_test);
  TEST(ring_buffer, multiple_write_with_peerk_and_read_test);
  TEST(ring_buffer, multiple_write_hitting_limit_test);
  TEST(ring_buffer, multiple_write_and_read_loop_test);

  auto rwlock = shakadb::test::RwLockTests();
  TEST(rwlock, double_read_lock_test);
  TEST(rwlock, upgrade_lock_test);
  TEST(rwlock, release_and_lock_again_test);

  auto monitor = shakadb::test::MonitorTests();
  TEST(monitor, create_delete_test);
  TEST(monitor, enter_test);
  TEST(monitor, enter_two_threads_test);

  auto configuration = shakadb::test::ConfigurationTests();
  TEST(configuration, init_test);
  TEST(configuration, full_test);

  auto database_performance = shakadb::test::DatabasePerformanceTests();
  TEST_PERF(database_performance, sequential_write_small);
  TEST_PERF(database_performance, sequential_write_medium);
  TEST_PERF(database_performance, sequential_write_large);
  TEST_PERF(database_performance, read_small);
  TEST_PERF(database_performance, read_medium);
  TEST_PERF(database_performance, read_large);
  TEST_PERF(database_performance, random_write_small);
  TEST_PERF(database_performance, random_write_medium);
  TEST_PERF(database_performance, random_write_large);

  auto database_concurrent = shakadb::test::DatabaseConcurrencyTests();
  TEST_PERF(database_concurrent, access_small);
  TEST_PERF(database_concurrent, access_medium);
  TEST_PERF(database_concurrent, access_large);

  runner.PrintSummary();
  printf("==================== Tests finished ===================\n");

  return result;
}
