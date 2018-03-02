//
// Created by Pawel Burzynski on 15/04/2017.
//

#ifndef TEST_SERVER_TESTS_H_
#define TEST_SERVER_TESTS_H_

#include "test/framework.h"

void test_server_simple_initialization_test(test_context_t ctx);
void test_server_connect(test_context_t ctx);
void test_server_connect_invalid_address(test_context_t ctx);
void test_server_connect_invalid_port(test_context_t ctx);
void test_server_write_small(test_context_t ctx);
void test_server_write_unordered(test_context_t ctx);
void test_server_write_two_batches(test_context_t ctx);
void test_server_read_multiple_batches(test_context_t ctx);
void test_server_read_range(test_context_t ctx);
void test_server_read_range_with_multiple_series(test_context_t ctx);
void test_server_update(test_context_t ctx);
void test_server_update_in_two_sessions(test_context_t ctx);
void test_server_truncate_not_existing(test_context_t ctx);
void test_server_truncate_empty(test_context_t ctx);
void test_server_truncate_and_write(test_context_t ctx);
void test_server_failed_write(test_context_t ctx);
void test_server_write_series_out_of_range(test_context_t ctx);
void test_server_read_series_out_of_range(test_context_t ctx);
void test_server_truncate_series_out_of_range(test_context_t ctx);
void test_server_write_filter_duplicates(test_context_t ctx);
void test_server_write_filter_zeros(test_context_t ctx);
void test_server_read_multiple_active(test_context_t ctx);
void test_server_read_latest_series_out_of_range(test_context_t ctx);
void test_server_read_latest_when_empty(test_context_t ctx);
void test_server_read_latest(test_context_t ctx);
void test_server_write_varsize(test_context_t ctx);

#endif  // TEST_SERVER_TESTS_H_
