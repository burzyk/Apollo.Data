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

#endif  // TEST_SERVER_TESTS_H_
