#include <cstdio>
#include <functional>
#include <test/framework/test-context.h>
#include <test/framework/test-runner.h>
#include <src/utils/directory.h>
#include "database-tests.h"

#define TEST(test_case) result = runner.RunTest("" #test_case "", test_case);

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

  runner.PrintSummary();
  printf("==================== Tests finished ===================\n");

  return result;
}