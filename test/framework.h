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
//
// Created by Pawel Burzynski on 15/04/2017.
//

#ifndef TEST_FRAMEWORK_H_
#define TEST_FRAMEWORK_H_

#include "src/common.h"

typedef struct test_session_s {
  char directory[SDB_STR_MAX_LEN];
  int tests_success;
  int tests_failed;
} test_session_t;

typedef struct test_context_s {
  char working_directory[SDB_STR_MAX_LEN];
  test_session_t *session;
  uint32_t point_size;
} test_context_t;

typedef void (*sdb_test_function_t)(test_context_t);

test_session_t *test_session_create(const char *root_directory);
void test_session_destroy(test_session_t *session);
int test_session_run(test_session_t *session, const char *name, sdb_test_function_t test_function);
void test_session_print_summary(test_session_t *session);

#endif  // TEST_FRAMEWORK_H_
