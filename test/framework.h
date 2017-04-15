//
// Created by Pawel Burzynski on 15/04/2017.
//

#ifndef TEST_FRAMEWORK_H_
#define TEST_FRAMEWORK_H_

#include <src/common.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif  // TEST_FRAMEWORK_H_
