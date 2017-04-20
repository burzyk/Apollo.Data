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

#include "test/framework.h"
#include "test/database-tests.h"
#include "test/server-tests.h"

#ifndef SDB_VERSION
#define SDB_VERSION "0.0.1"
#endif

#ifndef SDB_BUILD
#define SDB_BUILD "<COMMIT_ID>"
#endif

#define TEST(test_case) result |= sdb_tests_session_run(session, #test_case, test_case);

int main(int argc, char *argv[]) {
  const char *root_directory = argc == 1
                               ? "/Users/pburzynski/shakadb-test/data/test-stuff"
                               : argv[1];

  sdb_tests_session_t *session = sdb_tests_session_create(root_directory);
  int result = 0;

  printf("==================== Running tests ====================\n");
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

  sdb_tests_session_print_summary(session);
  printf("==================== Tests finished ===================\n");

  sdb_tests_session_destroy(session);
  return result;
}
