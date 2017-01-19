#include <cstdio>
#include <functional>
#include <test/framework/test-context.h>
#include <test/framework/test-runner.h>
#include <src/utils/directory.h>
#include "database-tests.h"

#define TEST(test_case) result = runner.RunTest("" #test_case "", test_case);

int main() {
  std::string dir = ""
  apollo::Directory::CreateDirectory("./test-result");
  apollo::test::TestRunner runner("./test-result");
  int result = 0;

  printf("==================== Running tests ====================\n");

  TEST(apollo::test::simple_database_initialization_test);
  TEST(apollo::test::basic_database_write_and_read_all);

  runner.PrintSummary();
  printf("==================== Tests finished ===================\n");

  return result;
}