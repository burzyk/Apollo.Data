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
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <src/client/client.h>
#include <src/utils/memory.h>
#include <src/utils/diagnostics.h>

#include "test/framework.h"

#ifndef SDB_VERSION
#define SDB_VERSION "0.0.1"
#endif

#ifndef SDB_BUILD
#define SDB_BUILD "<COMMIT_ID>"
#endif

#define SDB_TEST_RANDOM_READ "random-read"
#define SDB_TEST_READ_WRITE "read-write"

void sdb_stress_test_read_write(const char *hostname, int port);
void sdb_stress_test_random_read(const char *hostname, int port);

int main(int argc, char *argv[]) {
  int configuration_parsed = 0;
  int port = 8487;
  char hostname[SDB_FILE_MAX_LEN];
  char test[SDB_FILE_MAX_LEN];
  strcpy(hostname, "localhost");
  strcpy(test, SDB_TEST_RANDOM_READ);

  while (!configuration_parsed) {
    int option_index = 0;
    static struct option long_options[] = {
        {"hostname", required_argument, 0, 'h'},
        {"port", required_argument, 0, 'p'},
        {"test", required_argument, 0, 't'}
    };

    int c = getopt_long(argc, argv, "h:p:t:", long_options, &option_index);

    if (c == -1) {
      configuration_parsed = 1;
    } else {
      switch (c) {
        case 'h':strncpy(hostname, optarg, SDB_FILE_MAX_LEN);
          break;
        case 'p':port = atoi(optarg);
          break;
        default: configuration_parsed = -1;
      }
    }
  }

  if (configuration_parsed == -1) {
    die("Invalid options");
  }

  printf("==================== Running stress tests ====================\n");
  printf("\n");
  printf("    Version: %s\n", SDB_VERSION);
  printf("    Build: %s\n", SDB_BUILD);
  printf("\n");
  printf("    Test: %s\n", test);
  printf("\n");
  printf("    Hostname: %s\n", hostname);
  printf("    Port: %d\n", port);
  printf("\n");

  srand((unsigned int)time(NULL));
  sdb_log_init(0);

  if (!strcmp(test, SDB_TEST_READ_WRITE)) {
    sdb_stress_test_read_write(hostname, port);
  }

  if (!strcmp(test, SDB_TEST_RANDOM_READ)) {
    sdb_stress_test_random_read(hostname, port);
  }

  printf("==================== Tests finished ===================\n");
}

void sdb_stress_test_random_read(const char *hostname, int port) {
  int max_tests = 1000;
  int max_reads = 100;
  int max_reads_repeat = 10;
  int points_batch_size = 1000000;
  int points_batch_count = 100;
  int read_count = 100000;
  int total_points = points_batch_count * points_batch_size;
  shakadb_data_point_t *points = (shakadb_data_point_t *)sdb_alloc(sizeof(shakadb_data_point_t) * points_batch_size);
  sdb_stopwatch_t *sw;

  while (max_tests--) {
    sdb_log_info("Connecting to server ...");
    shakadb_session_t session;
    sdb_assert(shakadb_session_open(&session, hostname, port) == SHAKADB_RESULT_OK, "Failed to connect")

    int series = rand() % 1000000;
    sdb_log_info("Testing series: %d", series);

    sdb_assert(shakadb_truncate_data_series(&session, series) == SHAKADB_RESULT_OK, "Failed to truncate data")

    sw = sdb_stopwatch_start();
    sdb_log_info("Seeding database ...");

    for (int i = 0; i < points_batch_count; i++) {
      for (shakadb_timestamp_t j = 0; j < points_batch_size; j++) {
        points[j].time = i * points_batch_size + j + 100;
        points[j].value = j + 100;
      }

      sdb_assert(
          shakadb_write_points(&session, series, points, points_batch_size) == SHAKADB_RESULT_OK,
          "Failed to write data");
    }

    sdb_log_info("> Seeded with: %d in: %fs", total_points, sdb_stopwatch_stop_and_destroy(sw));

    for (int i = 0; i < max_reads; i++) {
      int begin = rand() % total_points - read_count;
      begin = sdb_max(begin, 1);
      int end = begin + read_count;

      for (int j = 0; j < max_reads_repeat; j++) {

        sdb_log_info("> Reading: [%d, %d)", begin, end);

        sw = sdb_stopwatch_start();
        shakadb_data_points_iterator_t it;

        sdb_assert(shakadb_read_points(&session, series, begin, end, &it) == SHAKADB_RESULT_OK, "Failed to read data");

        while (shakadb_data_points_iterator_next(&it)) {
        }

        sdb_log_info("> Read: [%d, %d) in: %fs", begin, end, sdb_stopwatch_stop_and_destroy(sw));
      }
    }

    sdb_log_info("Closing session ...");
    shakadb_session_close(&session);
  }

  sdb_free(points);
}

void sdb_stress_test_read_write(const char *hostname, int port) {
  int max_tests = 1000;
  int max_reads = 10;
  int points_count = 10000;
  int step = 2;
  int max_count = 5000000;
  sdb_stopwatch_t *sw;

  while (max_tests--) {
    sdb_log_info("Connecting to server ...");
    shakadb_session_t session;
    sdb_assert(shakadb_session_open(&session, hostname, port) == SHAKADB_RESULT_OK, "Failed to connect")

    int series = rand() % 1000000;
    sdb_log_info("Testing series: %d", series);

    while (points_count < max_count) {
      sw = sdb_stopwatch_start();
      sdb_assert(shakadb_truncate_data_series(&session, series) == SHAKADB_RESULT_OK, "Failed to truncate data")
      sdb_log_info("> Truncated in: %fs", sdb_stopwatch_stop_and_destroy(sw));

      shakadb_data_point_t *points = (shakadb_data_point_t *)sdb_alloc(sizeof(shakadb_data_point_t) * points_count);

      for (shakadb_timestamp_t i = 0; i < points_count; i++) {
        points[i].time = i + 100;
        points[i].value = i;
      }

      sw = sdb_stopwatch_start();
      sdb_assert(
          shakadb_write_points(&session, series, points, points_count) == SHAKADB_RESULT_OK,
          "Failed to write data")
      sdb_log_info("> Written in: %fs", sdb_stopwatch_stop_and_destroy(sw));

      for (int i = 0; i < max_reads; i++) {
        sw = sdb_stopwatch_start();
        shakadb_data_points_iterator_t it;

        sdb_assert(
            shakadb_read_points(&session, series, SHAKADB_MIN_TIMESTAMP, SHAKADB_MAX_TIMESTAMP, &it)
                == SHAKADB_RESULT_OK,
            "Failed to read data");

        int expected_pos = 0;
        int actual_pos = 0;

        while (shakadb_data_points_iterator_next(&it)) {
          while (expected_pos < points_count && actual_pos < it.points_count) {
            sdb_assert(it.points[actual_pos].time == points[expected_pos].time, "Invalid time");
            sdb_assert(it.points[actual_pos].value == points[expected_pos].value, "Invalid value");

            actual_pos++;
            expected_pos++;
          }

          actual_pos = 0;
        }

        sdb_assert(expected_pos == points_count, "Not all points have been read");
        sdb_log_info("> Read: %d in: %fs", points_count, sdb_stopwatch_stop_and_destroy(sw));
      }

      sdb_free(points);
      points_count *= step;
    }

    sdb_log_info("Closing session ...");
    points_count = 10000;
    shakadb_session_close(&session);
  }
}
