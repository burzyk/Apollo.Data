//
// Created by Pawel Burzynski on 15/04/2017.
//

#include "test/framework.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "src/utils/memory.h"
#include "src/utils/disk.h"

sdb_tests_session_t *sdb_tests_session_create(const char *root_directory) {
  sdb_tests_session_t *session = (sdb_tests_session_t *)sdb_alloc(sizeof(sdb_tests_session_t));
  snprintf(session->_directory, SDB_FILE_MAX_LEN, "%s/%lu", root_directory, time(NULL));
  session->_tests_failed = 0;
  session->_tests_success = 0;

  sdb_assert(!sdb_directory_create(session->_directory), "Unable to create directory for tests");

  return session;
}

void sdb_tests_session_destroy(sdb_tests_session_t *session) {
  sdb_free(session);
}

int sdb_tests_session_run(sdb_tests_session_t *session, const char *name, sdb_test_function_t test_function) {
  printf("Running: %s ...", name);

  char working_directory[SDB_FILE_MAX_LEN] = {0};
  snprintf(working_directory, SDB_FILE_MAX_LEN, "%s/%s", session->_directory, name);

  sdb_assert(!sdb_directory_create(working_directory), "Unable to create working directory")
  sdb_tests_context_t context = {};
  strncpy(context.working_directory, working_directory, SDB_FILE_MAX_LEN);
  context.session = session;

  printf(ANSI_COLOR_RESET);
  int result = 1;
  test_function(context);

  if (result) {
    printf(ANSI_COLOR_GREEN " [ OK ]\n");
    session->_tests_success++;
  } else {
    printf(ANSI_COLOR_RED " [ Failed ]\n");
    session->_tests_failed++;
  }

  printf(ANSI_COLOR_RESET);
  return result;
}

void sdb_tests_session_print_summary(sdb_tests_session_t *session) {
  printf("Test run: %d, failed: %d\n", session->_tests_success + session->_tests_failed, session->_tests_failed);
}