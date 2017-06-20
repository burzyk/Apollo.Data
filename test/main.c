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
#include "test/database-tests.h"
#include "test/server-tests.h"

#ifndef SDB_VERSION
#define SDB_VERSION "0.0.1"
#endif

#ifndef SDB_BUILD
#define SDB_BUILD "<COMMIT_ID>"
#endif

#define SDB_TEST_MODE_UNIT "unit_tests"
#define SDB_TEST_MODE_STRESS "stress_tests"

#define TEST(test_case) result |= sdb_tests_session_run(session, #test_case, test_case);

typedef struct sdb_test_configuration_s {
  char mode[SDB_FILE_MAX_LEN];
  char directory[SDB_FILE_MAX_LEN];
  int port;
  char hostname[SDB_FILE_MAX_LEN];
} sdb_test_configuration_t;

int sdb_run_unit_tests(const char *root_directory);
void sdb_test_configuration_parse(sdb_test_configuration_t *configuration, int argc, char *argv[]);
int sdb_run_stress_tests(const char *hostname, int port);

int main(int argc, char *argv[]) {
  sdb_test_configuration_t config;

  strcpy(config.mode, SDB_TEST_MODE_STRESS);
  strcpy(config.directory, "/Users/pburzynski/shakadb-test/data/test-stuff");
  config.port = 8487;
  strcpy(config.hostname, "localhost");

  sdb_test_configuration_parse(&config, argc, argv);

  if (!strcmp(SDB_TEST_MODE_UNIT, config.mode)) {
    return sdb_run_unit_tests(config.directory);
  }

  if (!strcmp(SDB_TEST_MODE_STRESS, config.mode)) {
    return sdb_run_stress_tests(config.hostname, config.port);
  }
}

void sdb_test_configuration_parse(sdb_test_configuration_t *config, int argc, char *argv[]) {
  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
        {"mode", optional_argument, 0, 'm'},
        {"directory", optional_argument, 0, 'd'},
        {"port", optional_argument, 0, 'p'},
        {"hostname", optional_argument, 0, 'h'}
    };

    int c = getopt_long(argc, argv, "m:d:p:h:", long_options, &option_index);
    if (c == -1) {
      return;
    }

    switch (c) {
      case 'm':strncpy(config->mode, optarg, SDB_FILE_MAX_LEN);
        break;
      case 'd':strncpy(config->directory, optarg, SDB_FILE_MAX_LEN);
        break;
      case 'p':config->port = atoi(optarg);
        break;
      case 'h':strncpy(config->hostname, optarg, SDB_FILE_MAX_LEN);
        break;
      default:return;
    }
  }
}

int sdb_run_stress_tests(const char *hostname, int port) {
  printf("==================== Running stress tests ====================\n");
  printf("\n");
  printf("    Version: %s\n", SDB_VERSION);
  printf("    Build: %s\n", SDB_BUILD);
  printf("\n");
  printf("    Hostname: %s\n", hostname);
  printf("    Port: %d\n", port);
  printf("\n");

  srand(time(NULL));
  sdb_log_init(0);

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

  printf("==================== Tests finished ===================\n");

  return 0;
}

int sdb_run_unit_tests(const char *root_directory) {
  sdb_tests_session_t *session = sdb_tests_session_create(root_directory);
  int result = 0;

  printf("==================== Running unit tests ====================\n");
  printf("\n");
  printf("    Version: %s\n", SDB_VERSION);
  printf("    Build: %s\n", SDB_BUILD);
  printf("\n");
  printf("    Directory: %s\n", root_directory);
  printf("\n");

  TEST(sdb_test_database_simple_initialization_test);
  TEST(sdb_test_database_write_and_read_all);
  TEST(sdb_test_database_write_database_in_one_big_batch);
  TEST(sdb_test_database_write_database_in_multiple_small_batches);
  TEST(sdb_test_database_multi_write_and_read_all);
  TEST(sdb_test_database_write_history);
  TEST(sdb_test_database_write_close_and_write_more);
  TEST(sdb_test_database_continuous_write);
  TEST(sdb_test_database_continuous_write_with_pickup);
  TEST(sdb_test_database_write_batch_size_equal_to_page_capacity);
  TEST(sdb_test_database_write_batch_size_greater_than_page_capacity);
  TEST(sdb_test_database_read_inside_single_chunk);
  TEST(sdb_test_database_read_span_two_chunks);
  TEST(sdb_test_database_read_span_three_chunks);
  TEST(sdb_test_database_read_chunk_edges);
  TEST(sdb_test_database_read_duplicated_values);
  TEST(sdb_test_database_read_with_limit);
  TEST(sdb_test_database_truncate);
  TEST(sdb_test_database_truncate_multiple);
  TEST(sdb_test_database_truncate_write_again);
  TEST(sdb_test_database_failed_write);

  TEST(sdb_test_server_simple_initialization_test);
  TEST(sdb_test_server_connect);
  TEST(sdb_test_server_connect_invalid_address);
  TEST(sdb_test_server_connect_invalid_port);
  TEST(sdb_test_server_write_small);
  TEST(sdb_test_server_write_unordered);
  TEST(sdb_test_server_write_two_batches);
  TEST(sdb_test_server_read_two_batches);
  TEST(sdb_test_server_read_range);
  TEST(sdb_test_server_read_range_with_multiple_series);
  TEST(sdb_test_server_update);
  TEST(sdb_test_server_update_in_two_sessions);
  TEST(sdb_test_server_truncate_not_existing);
  TEST(sdb_test_server_truncate_empty);
  TEST(sdb_test_server_truncate_and_write);
  TEST(sdb_test_server_no_sig_pipe_on_too_large_packet);
  TEST(sdb_test_server_failed_write);
  TEST(sdb_test_server_write_series_out_of_range);
  TEST(sdb_test_server_read_series_out_of_range);
  TEST(sdb_test_server_truncate_series_out_of_range);

  sdb_tests_session_print_summary(session);
  printf("==================== Tests finished ===================\n");

  sdb_tests_session_destroy(session);
  return result;
}
