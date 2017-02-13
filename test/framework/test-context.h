//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef SHAKADB_STORAGE_TEST_CONTEXT_H
#define SHAKADB_STORAGE_TEST_CONTEXT_H

#include <string>
#include <functional>

namespace shakadb {
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

#endif //SHAKADB_STORAGE_TEST_CONTEXT_H
