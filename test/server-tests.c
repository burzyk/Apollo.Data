//
// Created by Pawel Burzynski on 15/04/2017.
//

#include <src/client/client.h>
#include "server-tests.h"

#include "src/server/server.h"

#define SDB_EUR_USD_ID  123487

void sdb_test_server_simple_initialization_test(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 100, db);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_connect(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 100, db);

  sdb_assert(shakadb_session_open(&session, "localhost", 8081) == SHAKADB_RESULT_OK, "Unable to connect");
  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_connect_invalid_address(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 100, db);

  int status = shakadb_session_open(&session, "sadasasdasdasd.asdas", 8081);
  sdb_assert(status == SHAKADB_RESULT_ERROR, "Should not connect");
  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_connect_invalid_port(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 100, db);

  int status = shakadb_session_open(&session, "localhost", 25876);
  sdb_assert(status == SHAKADB_RESULT_ERROR, "Should not connect");
  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_write_small(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 100, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points[] = {
      {.time=3, .value = 13},
      {.time=5, .value = 76},
      {.time=15, .value = 44}
  };

  status = shakadb_write_points(&session, SDB_EUR_USD_ID, points, 3);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  shakadb_data_points_iterator_t it = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, SHAKADB_MIN_TIMESTAMP, SHAKADB_MAX_TIMESTAMP, &it);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it.points_count == 3, "Invalid number of points");
  sdb_assert(it.points[0].time == 3, "Time should be 3");
  sdb_assert(it.points[1].time == 5, "Time should be 5");
  sdb_assert(it.points[2].time == 15, "Time should be 15");
  sdb_assert(it.points[0].value == 13, "Value should be 13");
  sdb_assert(it.points[1].value == 76, "Value should be 76");
  sdb_assert(it.points[2].value == 44, "Value should be 44.3");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status == 0, "Data left in iterator")

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

// Write with invalid order
