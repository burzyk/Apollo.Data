//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef APOLLO_STORAGE_TEST_RUNNER_H
#define APOLLO_STORAGE_TEST_RUNNER_H

#include <src/utils/stopwatch.h>
#include "test-context.h"

namespace apollo {
namespace test {

class TestRunner {
 public:
  TestRunner(std::string directory);

  int RunTest(std::string name, std::function<void(TestContext)> func);
  int RunPerfTest(std::string name, std::function<Stopwatch(TestContext)> func);

  void PrintSummary();
 private:
  std::string directory;
  int tests_success;
  int tests_failed;
};

}
}

#endif //APOLLO_STORAGE_TESTRUNNER_H
