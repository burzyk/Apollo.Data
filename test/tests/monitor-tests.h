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
// Created by Pawel Burzynski on 14/02/2017.
//

#ifndef TEST_TESTS_MONITOR_TESTS_H_
#define TEST_TESTS_MONITOR_TESTS_H_

#include <list>
#include <vector>

#include "src/utils/monitor.h"
#include "src/utils/thread.h"
#include "test/framework/assert.h"
#include "test/framework/test-context.h"

namespace shakadb {
namespace test {

class MonitorTests {
 public:
  void create_delete_test(TestContext ctx);
  void enter_test(TestContext ctx);
  void enter_two_threads_test(TestContext ctx);
};

}  // namespace test
}  // namespace shakadb

#endif  // TEST_TESTS_MONITOR_TESTS_H_
