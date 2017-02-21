#include <cstdio>
#include <functional>
#include <test/framework/test-context.h>
#include <test/framework/test-runner.h>
#include <src/utils/directory.h>
#include <test/utils/ring-buffer-tests.h>
#include <src/utils/allocator.h>
#include <test/utils/monitor-tests.h>
#include "test/storage/common.h"
#include "test/storage/unit-tests.h"
#include "test/storage/performance-tests.h"
#include "test/storage/concurrency-tests.h"
#include "test/utils/rw-lock-tests.h"
#include "test/domain/configuration-tests.h"
#include "test/server/end-to-end.h"

#define RUN_TESTS
//#define RUN_PERF_TESTS

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
  std::string dir("/Users/pburzynski/shakadb-test/data/test-stuff");
  shakadb::Directory::CreateDirectory(dir);
  shakadb::test::TestRunner runner(dir);
  int result = 0;

  printf("==================== Running tests ====================\n");

  TEST(shakadb::test::simple_database_initialization_test);
  TEST(shakadb::test::basic_database_write_and_read_all);
  TEST(shakadb::test::write_database_in_one_big_batch);
  TEST(shakadb::test::write_database_in_multiple_small_batches);
  TEST(shakadb::test::database_write_history);
  TEST(shakadb::test::database_write_close_and_write_more);
  TEST(shakadb::test::database_multi_write_and_read_all);
  TEST(shakadb::test::database_continuous_write);
  TEST(shakadb::test::database_continuous_write_with_pickup);
  TEST(shakadb::test::database_write_batch_size_equal_to_page_capacity);
  TEST(shakadb::test::database_write_batch_size_greater_than_page_capacity);
  TEST(shakadb::test::database_read_inside_single_chunk);
  TEST(shakadb::test::database_read_span_two_chunks);
  TEST(shakadb::test::database_read_span_three_chunks);
  TEST(shakadb::test::database_read_chunk_edges);
  TEST(shakadb::test::database_read_duplicated_values);

  TEST(shakadb::test::ring_buffer_create_delete_test);
  TEST(shakadb::test::ring_buffer_empty_read_test);
  TEST(shakadb::test::ring_buffer_empty_peek_test);
  TEST(shakadb::test::ring_buffer_simple_write_test);
  TEST(shakadb::test::ring_buffer_multiple_write_with_peerk_and_read_test);
  TEST(shakadb::test::ring_buffer_multiple_write_hitting_limit_test);
  TEST(shakadb::test::ring_buffer_multiple_write_and_read_loop_test);

  TEST(shakadb::test::rwlock_double_read_lock_test);
  TEST(shakadb::test::rwlock_upgrade_lock_test);
  TEST(shakadb::test::rwlock_release_and_lock_again_test);

  TEST(shakadb::test::monitor_create_delete_test);
  TEST(shakadb::test::monitor_enter_test);
  TEST(shakadb::test::monitor_enter_two_threads_test);

  TEST(shakadb::test::configuration_init_test);
  TEST(shakadb::test::configuration_full_test);

//  TEST_PERF(shakadb::test::database_performance_sequential_write_small);
//  TEST_PERF(shakadb::test::database_performance_sequential_write_medium);
//  TEST_PERF(shakadb::test::database_performance_sequential_write_large);
//  TEST_PERF(shakadb::test::database_performance_read_small);
//  TEST_PERF(shakadb::test::database_performance_read_medium);
//  TEST_PERF(shakadb::test::database_performance_read_large);
//  TEST_PERF(shakadb::test::database_performance_random_write_small);
//  TEST_PERF(shakadb::test::database_performance_random_write_medium);
//  TEST_PERF(shakadb::test::database_performance_random_write_large);
//
//  TEST_PERF(shakadb::test::database_concurrent_access_small);
//  TEST_PERF(shakadb::test::database_concurrent_access_medium);
//  TEST_PERF(shakadb::test::database_concurrent_access_large);

  //TEST_PERF(shakadb::test::e2e_initial_write);

  runner.PrintSummary();
  printf("==================== Tests finished ===================\n");

  shakadb::Allocator::AssertAllDeleted();

  return result;
}