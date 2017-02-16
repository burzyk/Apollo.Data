//
// Created by Pawel Burzynski on 19/01/2017.
//

#include <string>
#include <src/utils/directory.h>
#include "test-runner.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

namespace shakadb {
namespace test {

TestRunner::TestRunner(std::string directory) {
  this->directory = directory;
  this->tests_failed = 0;
  this->tests_success = 0;
}

int TestRunner::RunTest(std::string name, std::function<void(TestContext)> func) {
  printf("Running: %s ...", name.c_str());

  try {
    std::string dir = this->directory + "/" + name;
    Directory::CreateDirectory(dir);
    TestContext ctx(dir);

    func(dir);

    printf(ANSI_COLOR_GREEN " [ OK ]\n" ANSI_COLOR_RESET);
    this->tests_success++;
    return 0;
  } catch (...) {
    printf(ANSI_COLOR_RED " [ Failed ]\n" ANSI_COLOR_RESET);
    this->tests_failed++;
    return -1;
  }
}

int TestRunner::RunPerfTest(std::string name, std::function<Stopwatch(TestContext)> func) {
  printf("Running performance: %s ...", name.c_str());

  try {
    std::string dir = this->directory + "/" + name;
    Directory::CreateDirectory(dir);
    TestContext ctx(dir);

    Stopwatch sw = func(dir);

    printf(ANSI_COLOR_GREEN " [ OK ]" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_GREEN " [ %fs ]\n" ANSI_COLOR_RESET, sw.GetElapsedSeconds());
    this->tests_success++;
    return 0;
  } catch (...) {
    printf(ANSI_COLOR_RED " [ Failed ]\n" ANSI_COLOR_RESET);
    this->tests_failed++;
    return -1;
  }
}

void TestRunner::PrintSummary() {
  printf("Test run: %d, failed: %d\n", this->tests_success + this->tests_failed, this->tests_failed);
}

}
}