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
#include <src/diagnostics.h>

#include "test/framework.h"
#include "test/database-tests.h"
#include "test/server-tests.h"
#include "test/common-tests.h"

#ifndef SDB_VERSION
#define SDB_VERSION "0.0.1"
#endif

#ifndef SDB_BUILD
#define SDB_BUILD "<COMMIT_ID>"
#endif

#define TEST(test_case) result |= test_session_run(session, #test_case, test_case);

int main(int argc, char *argv[]) {
  int configuration_parsed = 0;
  char directory[SDB_FILE_MAX_LEN] = {0};
  strcpy(directory, "/home/pawel/projects/ShakaDB/build/tests");

  while (!configuration_parsed) {
    int option_index = 0;
    static struct option long_options[] = {
        {"directory", required_argument, 0, 'd'}
    };

    int c = getopt_long(argc, argv, "d:", long_options, &option_index);

    if (c == -1) {
      configuration_parsed = 1;
    } else {
      switch (c) {
        case 'd':strncpy(directory, optarg, SDB_FILE_MAX_LEN);
          break;
        default: configuration_parsed = -1;
      }
    }
  }

  if (configuration_parsed == -1) {
    die("invalid arguments");
  }

  test_session_t *session = test_session_create(directory);
  int result = 0;

  printf("==================== Running unit tests ====================\n");
  printf("\n");
  printf("    Version: %s\n", SDB_VERSION);
  printf("    Build: %s\n", SDB_BUILD);
  printf("\n");
  printf("    Directory: %s\n", directory);
  printf("\n");

  TEST(test_search_empty);
  TEST(test_search_left_out);
  TEST(test_search_right_out);
  TEST(test_search_left_approx);
  TEST(test_search_right_approx);
  TEST(test_search_exactly);
  TEST(test_search_even);
  TEST(test_search_odd);
  TEST(test_search_duplicates);

  TEST(test_database_simple_initialization_test);
  TEST(test_database_write_and_read_all);
  TEST(test_database_write_database_in_one_big_batch);
  TEST(test_database_write_database_in_multiple_small_batches);
  TEST(test_database_multi_write_and_read_all);
  TEST(test_database_write_history);
  TEST(test_database_write_close_and_write_more);
  TEST(test_database_continuous_write);
  TEST(test_database_continuous_write_with_pickup);
  TEST(test_database_write_batch_size_equal_to_page_capacity);
  TEST(test_database_write_batch_size_greater_than_page_capacity);
  TEST(test_database_read_inside_single_chunk);
  TEST(test_database_read_span_two_chunks);
  TEST(test_database_read_span_three_chunks);
  TEST(test_database_read_chunk_edges);
  TEST(test_database_read_duplicated_values);
  TEST(test_database_read_with_limit);
  TEST(test_database_truncate);
  TEST(test_database_truncate_multiple);
  TEST(test_database_truncate_write_again);
  TEST(test_database_failed_write);
  TEST(test_database_read_latest_no_data);
  TEST(test_database_read_latest_data_in_first_chunk);
  TEST(test_database_read_latest_data_in_second_chunk);

  TEST(test_server_simple_initialization_test);
  TEST(test_server_connect);
  TEST(test_server_connect_invalid_address);
  TEST(test_server_connect_invalid_port);
  TEST(test_server_write_small);
  TEST(test_server_write_unordered);
  TEST(test_server_write_two_batches);
  TEST(test_server_read_two_batches);
  TEST(test_server_read_range);
  TEST(test_server_read_range_with_multiple_series);
  TEST(test_server_update);
  TEST(test_server_update_in_two_sessions);
  TEST(test_server_truncate_not_existing);
  TEST(test_server_truncate_empty);
  TEST(test_server_truncate_and_write);
  TEST(test_server_failed_write);
  TEST(test_server_write_series_out_of_range);
  TEST(test_server_read_series_out_of_range);
  TEST(test_server_truncate_series_out_of_range);
  TEST(test_server_write_filter_duplicates);
  TEST(test_server_write_filter_zeros);
  TEST(test_server_read_multiple_active);
  TEST(test_server_read_latest_series_out_of_range);
  TEST(test_server_read_latest_when_empty);
  TEST(test_server_read_latest);

  test_session_print_summary(session);
  printf("==================== Tests finished ===================\n");

  test_session_destroy(session);
  return result;
}
