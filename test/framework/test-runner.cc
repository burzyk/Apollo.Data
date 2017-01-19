//
// Created by Pawel Burzynski on 19/01/2017.
//

#include <string>
#include <src/utils/directory.h>
#include "test-runner.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

namespace apollo {
namespace test {

TestRunner::TestRunner(std::string directory) {
  this->directory = directory;
}

int TestRunner::RunTest(std::string name, std::function<void(TestContext)> func) {
  printf(ANSI_COLOR_GREEN "Running: %s ..." ANSI_COLOR_RESET, name.c_str());

  try {
    std::string dir = this->directory + "/" + name;
    Directory::CreateDirectory(dir);
    TestContext ctx(dir);

    func(dir);

    printf(ANSI_COLOR_GREEN " OK\n" ANSI_COLOR_RESET);
    return 0;
  } catch (...) {
    printf(ANSI_COLOR_RED " Failed\n" ANSI_COLOR_RESET);
    return -1;
  }
}

}
}