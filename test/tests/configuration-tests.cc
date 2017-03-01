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
// Created by Pawel Burzynski on 01/03/2017.
//

#include "test/tests/configuration-tests.h"

#include <string>
#include <fstream>

namespace shakadb {
namespace test {

void ConfigurationTests::init_test(TestContext ctx) {
  Configuration *config = Configuration::Load("/dummy_file");

  Assert::IsTrue(config->GetServerBacklog() == 10);

  delete config;
}

void ConfigurationTests::full_test(TestContext ctx) {
  std::string config_raw("# this is an example configuration file\n"
      "# all values here are defaults\n"
      "log.file = some_log\n"
      "server.port = 1010\n"
      "server.backlog = 99\n"
      "db.folder = some_bad_folder\n"
      "\n"
      "# Number of data points per chunk in data file\n"
      "db.points_per_chunk = 2\n"
      "\n"
      "# Size of the buffer used when receiving the data points\n"
      "# for insertion\n"
      "write_handler.buffer_size = 4\n"
      "\n"
      "# Size of the buffer used when sending the data points\n"
      "# to the client\n"
      "read_handler.buffer_size = 7"
      "");

  std::string config_file = ctx.GetWorkingDirectory() + "/config.cfg";
  std::fstream f(config_file);
  f << config_raw;
  f.close();

  Configuration *config = Configuration::Load(config_file);

  Assert::IsTrue(config->GetLogFile() == "some_log");
  Assert::IsTrue(config->GetServerPort() == 1010);
  Assert::IsTrue(config->GetServerBacklog() == 99);
  Assert::IsTrue(config->GetDbFolder() == "some_bad_folder");
  Assert::IsTrue(config->GetDbPointsPerChunk() == 2);

  delete config;
}

}  // namespace test
}  // namespace shakadb
