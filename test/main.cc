#include <cstdio>
#include <functional>
#include <test/framework/test-context.h>
#include <test/framework/test-runner.h>
#include <src/utils/directory.h>
#include <test/tests/ring-buffer-tests.h>
#include <src/utils/allocator.h>
#include <test/tests/monitor-tests.h>
#include "test/tests/common.h"
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
      "" #clazz " -> " #test_case "",\
      [&clazz](shakadb::test::TestContext ctx) -> void { clazz.test_case(ctx); });
#else
#define TEST(test_case)
#endif

#ifdef RUN_PERF_TESTS
#define TEST_PERF(clazz, test_case) result |= runner.RunPerfTest(\
      "" #clazz " -> " #test_case "",\
      [&clazz](shakadb::test::TestContext ctx) -> shakadb::Stopwatch { return clazz.test_case(ctx); });
#else
#define TEST_PERF(test_case)
#endif

int main() {
  std::string dir("/Users/pburzynski/shakadb-test/data/test-stuff");
  shakadb::Directory::CreateDirectory(dir);
  shakadb::test::TestRunner runner(dir);
  int result = 0;

  printf("==================== Running tests ====================\n");

  auto context_factory = shakadb::test::StandardDatabaseContextFactory();

  auto database_basic = shakadb::test::DatabaseBasicTests(&context_factory);
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

  auto database_performance = shakadb::test::DatabasePerformanceTests(&context_factory);
  TEST_PERF(database_performance, sequential_write_small);
  TEST_PERF(database_performance, sequential_write_medium);
  TEST_PERF(database_performance, sequential_write_large);
  TEST_PERF(database_performance, read_small);
  TEST_PERF(database_performance, read_medium);
  TEST_PERF(database_performance, read_large);
  TEST_PERF(database_performance, random_write_small);
  TEST_PERF(database_performance, random_write_medium);
  TEST_PERF(database_performance, random_write_large);

  auto database_concurrent = shakadb::test::DatabaseConcurrencyTests(&context_factory);
  TEST_PERF(database_concurrent, access_small);
  TEST_PERF(database_concurrent, access_medium);
  TEST_PERF(database_concurrent, access_large);

  runner.PrintSummary();
  printf("==================== Tests finished ===================\n");

  shakadb::Allocator::AssertAllDeleted();

  return result;
}