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

typedef struct sdb_tests_session_s {
  char _directory[SDB_FILE_MAX_LEN];
  int _tests_success;
  int _tests_failed;
} sdb_tests_session_t;

typedef struct sdb_tests_context_s {
  char working_directory[SDB_FILE_MAX_LEN];
  sdb_tests_session_t *session;
} sdb_tests_context_t;

typedef void (*sdb_test_function_t)(sdb_tests_context_t);

sdb_tests_session_t *sdb_tests_session_create(const char *root_directory);
void sdb_tests_session_destroy(sdb_tests_session_t *session);
int sdb_tests_session_run(sdb_tests_session_t *session, const char *name, sdb_test_function_t test_function);
void sdb_tests_session_print_summary(sdb_tests_session_t *session);

#endif  // TEST_FRAMEWORK_H_
