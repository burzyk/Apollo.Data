#include <cstdio>
#include <functional>
#include <test/framework/test-context.h>
#include <test/framework/test-runner.h>
#include <src/utils/directory.h>
#include "database-tests.h"

#define TEST(test_case) result = runner.RunTest("" #test_case "", test_case);

int main() {
  apollo::Directory::CreateDirectory("./test-result");
  apollo::test::TestRunner runner("./test-result");
  int result = 0;

  TEST(apollo::test::simple_database_initialization_test);

  return result;
}