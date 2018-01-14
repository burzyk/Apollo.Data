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
#include <src/diagnostics.h>

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
        case 't':strncpy(test, optarg, SDB_FILE_MAX_LEN);
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
  log_init(0);

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
  stopwatch_t *sw;

  while (max_tests--) {
    log_info("Connecting to network ...");
    shakadb_session_t session;
    sdb_assert(shakadb_session_open(&session, hostname, port) == SHAKADB_RESULT_OK, "Failed to connect");

    shakadb_data_series_id_t series = rand() % 1000000U;
    log_info("Testing series: %d", series);

    sdb_assert(shakadb_truncate_data_series(&session, series) == SHAKADB_RESULT_OK, "Failed to truncate data");

    sw = stopwatch_start();
    log_info("Seeding database ...");

    for (int i = 0; i < points_batch_count; i++) {
      for (shakadb_timestamp_t j = 0; j < points_batch_size; j++) {
        points[j].time = i * points_batch_size + j + 100;
        points[j].value = j + 100;
      }

      sdb_assert(
          shakadb_write_points(&session, series, points, points_batch_size) == SHAKADB_RESULT_OK,
          "Failed to write data");
    }

    log_info("> Seeded with: %d in: %fs", total_points, stopwatch_stop_and_destroy(sw));

    for (int i = 0; i < max_reads; i++) {
      shakadb_timestamp_t begin = sdb_maxl((uint64_t)(rand() % total_points - read_count), 1);
      shakadb_timestamp_t end = begin + read_count;

      for (int j = 0; j < max_reads_repeat; j++) {

        log_info("> Reading: [%d, %d)", begin, end);

        sw = stopwatch_start();
        shakadb_data_points_iterator_t it;

        sdb_assert(
            shakadb_read_points(&session, series, begin, end, SDB_POINTS_PER_PACKET_MAX, &it) == SHAKADB_RESULT_OK,
            "Failed to read data");

        while (shakadb_data_points_iterator_next(&it)) {
        }

        log_info("> Read: [%d, %d) in: %fs", begin, end, stopwatch_stop_and_destroy(sw));
      }
    }

    log_info("Closing session ...");
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
  int status = 0;
  stopwatch_t *sw;

  while (max_tests--) {
    log_info("Connecting to network ...");
    shakadb_session_t session;
    sdb_assert(shakadb_session_open(&session, hostname, port) == SHAKADB_RESULT_OK, "Failed to connect");

    shakadb_data_series_id_t series = rand() % 1000000U;
    log_info("Testing series: %d", series);

    while (points_count < max_count) {
      sw = stopwatch_start();
      sdb_assert(shakadb_truncate_data_series(&session, series) == SHAKADB_RESULT_OK, "Failed to truncate data");
      log_info("> Truncated in: %fs", stopwatch_stop_and_destroy(sw));

      shakadb_data_point_t *points = (shakadb_data_point_t *)sdb_alloc(sizeof(shakadb_data_point_t) * points_count);

      for (shakadb_timestamp_t i = 0; i < points_count; i++) {
        points[i].time = i + 100;
        points[i].value = i;
      }

      sw = stopwatch_start();
      sdb_assert(
          shakadb_write_points(&session, series, points, points_count) == SHAKADB_RESULT_OK,
          "Failed to write data");
      log_info("> Written in: %fs", stopwatch_stop_and_destroy(sw));

      for (int i = 0; i < max_reads; i++) {
        sw = stopwatch_start();
        shakadb_data_points_iterator_t it;

        status = shakadb_read_points(&session,
                                     series,
                                     SHAKADB_MIN_TIMESTAMP,
                                     SHAKADB_MAX_TIMESTAMP,
                                     SDB_POINTS_PER_PACKET_MAX,
                                     &it);
        sdb_assert(status == SHAKADB_RESULT_OK, "Failed to read data");

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
        log_info("> Read: %d in: %fs", points_count, stopwatch_stop_and_destroy(sw));
      }

      sdb_free(points);
      points_count *= step;
    }

    log_info("Closing session ...");
    points_count = 10000;
    shakadb_session_close(&session);
  }
}
