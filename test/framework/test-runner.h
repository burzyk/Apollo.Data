//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef APOLLO_STORAGE_TEST_RUNNER_H
#define APOLLO_STORAGE_TEST_RUNNER_H

#include "test-context.h"

namespace apollo {
namespace test {

class TestRunner {
 public:
  TestRunner(std::string directory);

  int RunTest(std::string name, std::function<void(TestContext)> func);
 private:
  std::string directory;
};

}
}

#endif //APOLLO_STORAGE_TESTRUNNER_H
