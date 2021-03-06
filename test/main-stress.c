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

#include "src/diagnostics.h"
#include "src/ctl/session.h"

#ifndef SDB_VERSION
#define SDB_VERSION "0.0.1"
#endif

#ifndef SDB_BUILD
#define SDB_BUILD "<COMMIT_ID>"
#endif

#define SDB_TEST_RANDOM_READ "random-read"
#define SDB_TEST_READ_WRITE "read-write"

void sdb_stress_test_random_read(const char *hostname, int port, uint32_t point_size);

int main(int argc, char *argv[]) {
  int configuration_parsed = 0;
  int port = 8487;
  char hostname[SDB_STR_MAX_LEN];
  char test[SDB_STR_MAX_LEN];
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
        case 'h':strncpy(hostname, optarg, SDB_STR_MAX_LEN);
          break;
        case 'p':port = atoi(optarg);
          break;
        case 't':strncpy(test, optarg, SDB_STR_MAX_LEN);
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

  if (!strcmp(test, SDB_TEST_RANDOM_READ)) {
    sdb_stress_test_random_read(hostname, port, 12);
  }

  printf("==================== Tests finished ===================\n");
}

void sdb_stress_test_random_read(const char *hostname, int port, uint32_t point_size) {
  int max_tests = 1000;
  int max_reads = 100;
  int max_reads_repeat = 10;
  uint32_t points_batch_size = 100000;
  int points_batch_count = 1000;
  int read_count = 100000;
  int total_points = points_batch_count * points_batch_size;
  data_point_t *points = (data_point_t *)sdb_alloc(point_size * points_batch_size);
  stopwatch_t *sw;

  while (max_tests--) {
    log_info("Connecting to network ...");
    session_t *session = session_create(hostname, port);
    sdb_assert(session != NULL, "Failed to connect");

    series_id_t series = rand() % 1000000U;
    log_info("Testing series: %d", series);

    sdb_assert(!session_truncate(session, series), "Failed to truncate data");

    sw = stopwatch_start();
    log_info("Seeding database ...");

    for (int i = 0; i < points_batch_count; i++) {
      points_list_t request_points = {
          .content = points,
          .point_size = point_size,
          .count = points_batch_size
      };

      data_point_t *curr = request_points.content;
      data_point_t *end = points_list_end(&request_points);
      timestamp_t j = 0;

      while (curr != end) {
        curr->time = i * points_batch_size + j + 100;
        float value = j + 100;
        memcpy(curr->value, &value, sizeof(value));

        j++;
        curr = data_point_next(&request_points, curr);
      }

      sdb_assert(!session_write(session, series, &request_points), "Failed to write data");
    }

    log_info("> Seeded with: %d in: %fs", total_points, stopwatch_stop_and_destroy(sw));

    for (int i = 0; i < max_reads; i++) {
      timestamp_t begin = sdb_max((uint64_t)(rand() % total_points - read_count), 1);
      timestamp_t end = begin + read_count;

      for (int j = 0; j < max_reads_repeat; j++) {

        log_info("> Reading: [%d, %d)", begin, end);

        sw = stopwatch_start();
        sdb_assert(!session_read(session, series, begin, end), "Failed to read data");

        while (session_read_next(session)) {
        }

        log_info("> Read: [%d, %d) in: %fs", begin, end, stopwatch_stop_and_destroy(sw));
      }
    }

    log_info("Closing session ...");
    session_destroy(session);
  }

  sdb_free(points);
}
