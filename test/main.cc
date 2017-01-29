#include <cstdio>
#include <functional>
#include <test/framework/test-context.h>
#include <test/framework/test-runner.h>
#include <src/utils/directory.h>
#include "test/engine/common.h"
#include "test/engine/unit-tests.h"
#include "test/engine/performance-tests.h"
#include "test/engine/concurrency-tests.h"
#include "test/utils/rw-lock-test.h"

#define RUN_TESTS
#define RUN_PERF_TESTS

#ifdef RUN_TESTS
#define TEST(test_case) result = runner.RunTest("" #test_case "", test_case);
#else
#define TEST(test_case)
#endif

#ifdef RUN_PERF_TESTS
#define TEST_PERF(test_case) result = runner.RunPerfTest("" #test_case "", test_case);
#else
#define TEST_PERF(test_case)
#endif

int main() {
  std::string dir("/Users/pburzynski/apollo-test/data/test-stuff");
  apollo::Directory::CreateDirectory(dir);
  apollo::test::TestRunner runner(dir);
  int result = 0;

  printf("==================== Running tests ====================\n");

  TEST(apollo::test::simple_database_initialization_test);
  TEST(apollo::test::basic_database_write_and_read_all);
  TEST(apollo::test::write_database_in_one_big_batch);
  TEST(apollo::test::write_database_in_multiple_small_batches);
  TEST(apollo::test::database_write_history);
  TEST(apollo::test::database_write_close_and_write_more);
  TEST(apollo::test::database_multi_write_and_read_all);
  TEST(apollo::test::database_continuous_write);
  TEST(apollo::test::database_continuous_write_with_pickup);
  TEST(apollo::test::database_write_batch_size_equal_to_page_capacity);
  TEST(apollo::test::database_write_batch_size_greater_than_page_capacity);
  TEST(apollo::test::database_read_inside_single_chunk);
  TEST(apollo::test::database_read_span_two_chunks);
  TEST(apollo::test::database_read_span_three_chunks);
  TEST(apollo::test::database_read_chunk_edges);
  TEST(apollo::test::database_read_duplicated_values);

  TEST(apollo::test::rwlock_double_read_lock_test);
  TEST(apollo::test::rwlock_upgrade_lock_test);
  TEST(apollo::test::rwlock_release_and_lock_again_test);

//  TEST_PERF(apollo::test::database_performance_sequential_write_small);
//  TEST_PERF(apollo::test::database_performance_sequential_write_medium);
//  TEST_PERF(apollo::test::database_performance_sequential_write_large);
//  TEST_PERF(apollo::test::database_performance_read_small);
//  TEST_PERF(apollo::test::database_performance_read_medium);
//  TEST_PERF(apollo::test::database_performance_read_large);
//  TEST_PERF(apollo::test::database_performance_random_write_small);
//  TEST_PERF(apollo::test::database_performance_random_write_medium);
//  TEST_PERF(apollo::test::database_performance_random_write_large);

  TEST_PERF(apollo::test::database_concurrent_access_small);
  TEST_PERF(apollo::test::database_concurrent_access_medium);
  TEST_PERF(apollo::test::database_concurrent_access_large);

  runner.PrintSummary();
  printf("==================== Tests finished ===================\n");

  return result;
}