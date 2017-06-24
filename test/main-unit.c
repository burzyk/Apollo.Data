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
#include "test/common-tests.h"

#ifndef SDB_VERSION
#define SDB_VERSION "0.0.1"
#endif

#ifndef SDB_BUILD
#define SDB_BUILD "<COMMIT_ID>"
#endif

#define TEST(test_case) result |= sdb_tests_session_run(session, #test_case, test_case);

int main(int argc, char *argv[]) {
  int configuration_parsed = 0;
  char directory[SDB_FILE_MAX_LEN] = {0};
  strcpy(directory, "/Users/pburzynski/shakadb-test/data/test-stuff");

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

  sdb_tests_session_t *session = sdb_tests_session_create(directory);
  int result = 0;

  printf("==================== Running unit tests ====================\n");
  printf("\n");
  printf("    Version: %s\n", SDB_VERSION);
  printf("    Build: %s\n", SDB_BUILD);
  printf("\n");
  printf("    Directory: %s\n", directory);
  printf("\n");

  TEST(sdb_test_search_empty);
  TEST(sdb_test_search_left_out);
  TEST(sdb_test_search_right_out);
  TEST(sdb_test_search_left_approx);
  TEST(sdb_test_search_right_approx);
  TEST(sdb_test_search_exactly);
  TEST(sdb_test_search_even);
  TEST(sdb_test_search_odd);
  TEST(sdb_test_search_duplicates);

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
  TEST(sdb_test_database_cache_cleanup);
  TEST(sdb_test_database_cache_cleanup_old);
  TEST(sdb_test_database_cache_smaller_than_chunk);

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
  TEST(sdb_test_server_write_filter_duplicates);
  TEST(sdb_test_server_write_filter_zeros);
  TEST(sdb_test_server_read_multiple_active);

  sdb_tests_session_print_summary(session);
  printf("==================== Tests finished ===================\n");

  sdb_tests_session_destroy(session);
  return result;
}
