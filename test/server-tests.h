//
// Created by Pawel Burzynski on 15/04/2017.
//

#ifndef TEST_SERVER_TESTS_H_
#define TEST_SERVER_TESTS_H_

#include "test/framework.h"

void sdb_test_server_simple_initialization_test(sdb_tests_context_t ctx);
void sdb_test_server_connect(sdb_tests_context_t ctx);
void sdb_test_server_connect_invalid_address(sdb_tests_context_t ctx);
void sdb_test_server_connect_invalid_port(sdb_tests_context_t ctx);
void sdb_test_server_write_small(sdb_tests_context_t ctx);
void sdb_test_server_write_unordered(sdb_tests_context_t ctx);
void sdb_test_server_write_two_batches(sdb_tests_context_t ctx);
void sdb_test_server_read_two_batches(sdb_tests_context_t ctx);
void sdb_test_server_read_range(sdb_tests_context_t ctx);
void sdb_test_server_read_range_with_multiple_series(sdb_tests_context_t ctx);
void sdb_test_server_update(sdb_tests_context_t ctx);
void sdb_test_server_update_in_two_sessions(sdb_tests_context_t ctx);
void sdb_test_server_truncate_not_existing(sdb_tests_context_t ctx);
void sdb_test_server_truncate_empty(sdb_tests_context_t ctx);
void sdb_test_server_truncate_and_write(sdb_tests_context_t ctx);
void sdb_test_server_no_sig_pipe_on_too_large_packet(sdb_tests_context_t ctx);

#endif  // TEST_SERVER_TESTS_H_
