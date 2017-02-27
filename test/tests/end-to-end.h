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
// Created by Pawel Burzynski on 21/02/2017.
//

#ifndef SHAKADB_END_TO_END_H
#define SHAKADB_END_TO_END_H

#include <src/utils/stopwatch.h>
#include <test/framework/test-context.h>
#include <src/bootstrapper.h>

namespace shakadb {
namespace test {

Bootstrapper *e2e_init(TestContext ctx) {
  std::string config_file_name = ctx.GetWorkingDirectory() + "/server.cfg";
  std::string config = "db.folder = " + ctx.GetWorkingDirectory() + "/data";
  File f(config_file_name);

  f.Write((byte_t *)config.c_str(), config.size());
  f.Flush();

  return Bootstrapper::Run(config_file_name);
}

Stopwatch e2e_initial_write(TestContext ctx) {
  return Stopwatch();
}

}
}

#endif //SHAKADB_END_TO_END_H
