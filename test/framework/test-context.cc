//
// Created by Pawel Burzynski on 19/01/2017.
//

#include "test-context.h"

namespace shakadb {
namespace test {

TestContext::TestContext(std::string workingDirectory) {
  this->wd = workingDirectory;
}

std::string TestContext::GetWorkingDirectory() {
  return this->wd;
}

}
}