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

#include "test/tests/end-to-end.h"

#include <string>
#include <fstream>

#include "src/utils/allocator.h"
#include "src/utils/stopwatch.h"
#include "src/bootstrapper.h"
#include "src/client/client.h"
#include "src/utils/directory.h"
#include "src/utils/file.h"
#include "src/data-point.h"
#include "test/framework/test-context.h"
#include "test/framework/assert.h"

namespace shakadb {
namespace test {

void EndToEnd::empty_read(TestContext ctx) {
  Bootstrapper *bootstrapper = this->BootstrapInit(ctx);
  this->ValidateRead("USD_AUD", 0);
  bootstrapper->Stop();
  delete bootstrapper;
}

void EndToEnd::write_small(TestContext ctx) {
  Bootstrapper *bootstrapper = this->BootstrapInit(ctx);
  this->WriteSequencialPoints("USD_AUD", 10, 100);
  this->WriteSequencialPoints("USD_PLN", 10, 100);
  this->WriteSequencialPoints("USD_GBP", 10, 100);
  this->WriteSequencialPoints("USD_CLP", 10, 100);

  this->ValidateRead("USD_AUD", 1000);
  this->ValidateRead("USD_PLN", 1000);
  this->ValidateRead("USD_GBP", 1000);
  this->ValidateRead("USD_CLP", 1000);

  bootstrapper->Stop();
  delete bootstrapper;
}

void EndToEnd::write_multiple(TestContext ctx) {
  Bootstrapper *bootstrapper = this->BootstrapInit(ctx);
  this->WriteSequencialPoints("USD_AUD", 1000, 100);
  this->WriteSequencialPoints("USD_PLN", 1000, 100);
  this->WriteSequencialPoints("USD_GBP", 1000, 100);
  this->WriteSequencialPoints("USD_CLP", 1000, 100);

  this->ValidateRead("USD_AUD", 100000);
  this->ValidateRead("USD_PLN", 100000);
  this->ValidateRead("USD_GBP", 100000);
  this->ValidateRead("USD_CLP", 100000);

  bootstrapper->Stop();
  delete bootstrapper;
}

void EndToEnd::write_stop_read(TestContext ctx) {
  Bootstrapper *bootstrapper = this->BootstrapInit(ctx);
  this->WriteSequencialPoints("USD_AUD", 1000, 100);
  this->ValidateRead("USD_AUD", 100000);
  bootstrapper->Stop();
  delete bootstrapper;

  bootstrapper = this->BootstrapInit(ctx);
  this->ValidateRead("USD_AUD", 100000);
  bootstrapper->Stop();
  delete bootstrapper;
}

Bootstrapper *EndToEnd::BootstrapInit(TestContext ctx) {
  std::string config_file_name = ctx.GetWorkingDirectory() + "/server.cfg";
  std::string db_folder = ctx.GetWorkingDirectory() + "/data";
  std::string config =
      "db.folder = " + db_folder + "\n" +
          "log.file = " + ctx.GetWorkingDirectory() + "/log.txt";

  Directory::CreateDirectory(db_folder);

  std::fstream f(config_file_name);
  f << config;
  f.close();

  return Bootstrapper::Run(config_file_name);
}

void EndToEnd::ValidateRead(std::string series_name, int expected_points_count) {
  shakadb_session_t session;
  shakadb_read_points_iterator_t iterator;
  int total_points = 0;

  Assert::IsTrue(shakadb_open_session(&session, "localhost", 8099) != SHAKADB_RESULT_ERROR);
  Assert::IsTrue(shakadb_read_points(
      &session,
      series_name.c_str(),
      SHAKADB_MIN_TIMESTAMP,
      SHAKADB_MAX_TIMESTAMP,
      &iterator) != SHAKADB_RESULT_ERROR);

  while (shakadb_read_points_iterator_next(&iterator)) {
    for (int i = 0; i < iterator.points_count - 1; i++) {
      Assert::IsTrue(iterator.points[i].time <= iterator.points[i + 1].time);
      total_points++;
    }

    total_points++;
  }

  Assert::IsTrue(expected_points_count == total_points);
  shakadb_destroy_session(&session);
}

void EndToEnd::WriteSequencialPoints(std::string series_name, int batch_size, int count) {
  data_point_t *batch = Allocator::New<data_point_t>(batch_size);
  timestamp_t time = 1;

  for (int i = 0; i < count; i++) {
    for (int j = 0; j < batch_size; j++) {
      batch[j].time = time++;
      batch[j].value = j;
    }

    this->WritePoints(series_name, batch, batch_size);
  }

  Allocator::Delete(batch);
}

void EndToEnd::WritePoints(std::string series_name, data_point_t *points, int points_count) {
  shakadb_session_t session;
  shakadb_data_point_t *shaka_points = reinterpret_cast<shakadb_data_point_t *>(points);

  Assert::IsTrue(shakadb_open_session(&session, "localhost", 8099) != SHAKADB_RESULT_ERROR);
  Assert::IsTrue(shakadb_write_points(
      &session,
      series_name.c_str(),
      shaka_points,
      points_count) != SHAKADB_RESULT_ERROR);
  shakadb_destroy_session(&session);
}

}  // namespace test
}  // namespace shakadb
