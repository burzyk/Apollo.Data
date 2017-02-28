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
#include <src/client/client.h>

namespace shakadb {
namespace test {

class EndToEnd {
 public:
  void empty_read(TestContext ctx) {
    Bootstrapper *bootstrapper = this->BootstrapInit(ctx);
    this->ValidateRead("USD_AUD", 0);
    bootstrapper->Stop();
    delete bootstrapper;
  }
 private:
  Bootstrapper *BootstrapInit(TestContext ctx) {
    std::string config_file_name = ctx.GetWorkingDirectory() + "/server.cfg";
    std::string db_folder = ctx.GetWorkingDirectory() + "/data";
    std::string config =
        "db.folder = " + db_folder + "\n" +
            "log.file = " + ctx.GetWorkingDirectory() + "/log.txt";
    Directory::CreateDirectory(db_folder);
    File f(config_file_name);

    f.Write((byte_t *)config.c_str(), config.size());
    f.Flush();

    return Bootstrapper::Run(config_file_name);
  }

  void ValidateRead(std::string series_name, int expected_points_count) {
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

  void WritePoints(std::string series_name, data_point_t *points, int points_count) {
    shakadb_session_t session;
    shakadb_data_point_t *shaka_points = (shakadb_data_point_t *)points;

    Assert::IsTrue(shakadb_open_session(&session, "localhost", 8099) != SHAKADB_RESULT_ERROR);
    Assert::IsTrue(shakadb_write_points(
        &session,
        series_name.c_str(),
        shaka_points,
        points_count) != SHAKADB_RESULT_ERROR);
    shakadb_destroy_session(&session);
  }
};

}
}

#endif //SHAKADB_END_TO_END_H
