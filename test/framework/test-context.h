//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef APOLLO_STORAGE_TEST_CONTEXT_H
#define APOLLO_STORAGE_TEST_CONTEXT_H

#include <string>
#include <functional>

namespace apollo {
namespace test {

class TestContext {
 public:
  TestContext(std::string workingDirectory);

  std::string GetWorkingDirectory();
 private:
  std::string wd;
};

}
}

#endif //APOLLO_STORAGE_TEST_CONTEXT_H
