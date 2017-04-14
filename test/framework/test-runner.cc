/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//
// Created by Pawel Burzynski on 19/01/2017.
//

#include "test/framework/test-runner.h"

#include <string>

#include "src/utils/disk.h"

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
    sdb_directory_create(dir.c_str());
    TestContext ctx(dir);

    func(ctx);

    printf(ANSI_COLOR_GREEN " [ OK ]\n" ANSI_COLOR_RESET);
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

}  // namespace test
}  // namespace shakadb
