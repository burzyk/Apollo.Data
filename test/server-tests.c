//
// Created by Pawel Burzynski on 15/04/2017.
//

#include <src/client/client.h>
#include <src/utils/memory.h>
#include "server-tests.h"

#include "src/server/server.h"

#define SDB_EUR_USD_ID  123487
#define SDB_EUR_GBP_ID  323487

void sdb_test_server_simple_initialization_test(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 100, db);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_connect(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 100, db);

  sdb_assert(shakadb_session_open(&session, "localhost", 8081) == SHAKADB_RESULT_OK, "Unable to connect");
  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_connect_invalid_address(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 100, db);

  int status = shakadb_session_open(&session, "sadasasdasdasd.asdas", 8081);
  sdb_assert(status == SHAKADB_RESULT_ERROR, "Should not connect");
  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_connect_invalid_port(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100, SDB_DATA_SERIES_MAX);
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
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100, SDB_DATA_SERIES_MAX);
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

void sdb_test_server_write_unordered(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 100, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points[] = {
      {.time=6, .value = 13},
      {.time=5, .value = 76},
      {.time=1, .value = 44},
      {.time=44, .value = 44}
  };

  status = shakadb_write_points(&session, SDB_EUR_USD_ID, points, 4);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  shakadb_data_points_iterator_t it = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, SHAKADB_MIN_TIMESTAMP, SHAKADB_MAX_TIMESTAMP, &it);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it.points_count == 4, "Invalid number of points");
  sdb_assert(it.points[0].time == 1, "Time should be 1");
  sdb_assert(it.points[1].time == 5, "Time should be 5");
  sdb_assert(it.points[2].time == 6, "Time should be 6");
  sdb_assert(it.points[3].time == 44, "Time should be 44");
  sdb_assert(it.points[0].value == 44, "Value should be 44");
  sdb_assert(it.points[1].value == 76, "Value should be 76");
  sdb_assert(it.points[2].value == 13, "Value should be 13");
  sdb_assert(it.points[3].value == 44, "Value should be 44");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status == 0, "Data left in iterator")

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_write_two_batches(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 100, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points_1[] = {
      {.time=1, .value = 1},
      {.time=4, .value = 2}
  };

  shakadb_data_point_t points_2[] = {
      {.time=2, .value = 3},
      {.time=5, .value = 4}
  };

  status = shakadb_write_points(&session, SDB_EUR_USD_ID, points_1, 2);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  status = shakadb_write_points(&session, SDB_EUR_USD_ID, points_2, 2);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  shakadb_data_points_iterator_t it = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, SHAKADB_MIN_TIMESTAMP, SHAKADB_MAX_TIMESTAMP, &it);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it.points_count == 4, "Invalid number of points");
  sdb_assert(it.points[0].time == 1, "Time should be 1");
  sdb_assert(it.points[1].time == 2, "Time should be 2");
  sdb_assert(it.points[2].time == 4, "Time should be 4");
  sdb_assert(it.points[3].time == 5, "Time should be 5");
  sdb_assert(it.points[0].value == 1, "Value should be 1");
  sdb_assert(it.points[1].value == 3, "Value should be 3");
  sdb_assert(it.points[2].value == 2, "Value should be 2");
  sdb_assert(it.points[3].value == 4, "Value should be 4");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status == 0, "Data left in iterator")

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_read_two_batches(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 2, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 2, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4}
  };

  status = shakadb_write_points(&session, SDB_EUR_USD_ID, points, 4);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  shakadb_data_points_iterator_t it = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, SHAKADB_MIN_TIMESTAMP, SHAKADB_MAX_TIMESTAMP, &it);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it.points_count == 2, "Invalid number of points");
  sdb_assert(it.points[0].time == 1, "Time should be 1");
  sdb_assert(it.points[1].time == 2, "Time should be 2");
  sdb_assert(it.points[0].value == 1, "Value should be 1");
  sdb_assert(it.points[1].value == 2, "Value should be 2");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it.points_count == 2, "Invalid number of points");
  sdb_assert(it.points[0].time == 3, "Time should be 3");
  sdb_assert(it.points[1].time == 4, "Time should be 4");
  sdb_assert(it.points[0].value == 3, "Value should be 3");
  sdb_assert(it.points[1].value == 4, "Value should be 4");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status == 0, "Data left in iterator")

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_read_range(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4},
      {.time=5, .value = 5}
  };

  status = shakadb_write_points(&session, SDB_EUR_USD_ID, points, 5);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  shakadb_data_points_iterator_t it = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, 2, 4, &it);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it.points_count == 2, "Invalid number of points");
  sdb_assert(it.points[0].time == 2, "Time should be 2");
  sdb_assert(it.points[1].time == 3, "Time should be 3");
  sdb_assert(it.points[0].value == 2, "Value should be 2");
  sdb_assert(it.points[1].value == 3, "Value should be 3");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status == 0, "Data left in iterator")

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_read_range_with_multiple_series(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points_1[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4},
      {.time=5, .value = 5}
  };

  shakadb_data_point_t points_2[] = {
      {.time=1, .value = 10},
      {.time=2, .value = 20},
      {.time=3, .value = 30},
      {.time=4, .value = 40},
      {.time=5, .value = 50}
  };

  status = shakadb_write_points(&session, SDB_EUR_USD_ID, points_1, 5);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  status = shakadb_write_points(&session, SDB_EUR_GBP_ID, points_2, 5);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  shakadb_data_points_iterator_t it = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, 2, 4, &it);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it.points_count == 2, "Invalid number of points");
  sdb_assert(it.points[0].time == 2, "Time should be 2");
  sdb_assert(it.points[1].time == 3, "Time should be 3");
  sdb_assert(it.points[0].value == 2, "Value should be 2");
  sdb_assert(it.points[1].value == 3, "Value should be 3");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status == 0, "Data left in iterator")

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_update(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points_1[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4},
      {.time=5, .value = 5}
  };

  status = shakadb_write_points(&session, SDB_EUR_USD_ID, points_1, 5);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  shakadb_data_points_iterator_t it_1 = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, 2, 4, &it_1);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it_1);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it_1.points_count == 2, "Invalid number of points");
  sdb_assert(it_1.points[0].time == 2, "Time should be 2");
  sdb_assert(it_1.points[1].time == 3, "Time should be 3");
  sdb_assert(it_1.points[0].value == 2, "Value should be 2");
  sdb_assert(it_1.points[1].value == 3, "Value should be 3");

  status = shakadb_data_points_iterator_next(&it_1);
  sdb_assert(status == 0, "Data left in iterator")

  shakadb_data_point_t points_2[] = {
      {.time=1, .value = 10},
      {.time=2, .value = 20},
      {.time=3, .value = 30},
      {.time=4, .value = 40},
      {.time=5, .value = 50}
  };

  status = shakadb_write_points(&session, SDB_EUR_USD_ID, points_2, 5);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  shakadb_data_points_iterator_t it_2 = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, 2, 4, &it_2);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it_2);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it_2.points_count == 2, "Invalid number of points");
  sdb_assert(it_2.points[0].time == 2, "Time should be 2");
  sdb_assert(it_2.points[1].time == 3, "Time should be 3");
  sdb_assert(it_2.points[0].value == 20, "Value should be 20");
  sdb_assert(it_2.points[1].value == 30, "Value should be 30");

  status = shakadb_data_points_iterator_next(&it_2);
  sdb_assert(status == 0, "Data left in iterator")

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_update_in_two_sessions(sdb_tests_context_t ctx) {
  shakadb_session_t session_1;
  shakadb_session_t session_2;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session_1, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points_1[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4},
      {.time=5, .value = 5}
  };

  status = shakadb_write_points(&session_1, SDB_EUR_USD_ID, points_1, 5);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  shakadb_data_points_iterator_t it_1 = {};
  status = shakadb_read_points(&session_1, SDB_EUR_USD_ID, 2, 4, &it_1);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it_1);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it_1.points_count == 2, "Invalid number of points");
  sdb_assert(it_1.points[0].time == 2, "Time should be 2");
  sdb_assert(it_1.points[1].time == 3, "Time should be 3");
  sdb_assert(it_1.points[0].value == 2, "Value should be 2");
  sdb_assert(it_1.points[1].value == 3, "Value should be 3");

  status = shakadb_data_points_iterator_next(&it_1);
  sdb_assert(status == 0, "Data left in iterator")

  shakadb_session_close(&session_1);

  status = shakadb_session_open(&session_2, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points_2[] = {
      {.time=1, .value = 10},
      {.time=2, .value = 20},
      {.time=3, .value = 30},
      {.time=4, .value = 40},
      {.time=5, .value = 50}
  };

  status = shakadb_write_points(&session_2, SDB_EUR_USD_ID, points_2, 5);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

  shakadb_data_points_iterator_t it_2 = {};
  status = shakadb_read_points(&session_2, SDB_EUR_USD_ID, 2, 4, &it_2);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it_2);
  sdb_assert(status != 0, "No data found in iterator")

  sdb_assert(it_2.points_count == 2, "Invalid number of points");
  sdb_assert(it_2.points[0].time == 2, "Time should be 2");
  sdb_assert(it_2.points[1].time == 3, "Time should be 3");
  sdb_assert(it_2.points[0].value == 20, "Value should be 20");
  sdb_assert(it_2.points[1].value == 30, "Value should be 30");

  status = shakadb_data_points_iterator_next(&it_2);
  sdb_assert(status == 0, "Data left in iterator")

  shakadb_session_close(&session_2);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_truncate_not_existing(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  status = shakadb_truncate_data_series(&session, SDB_EUR_USD_ID);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when truncating");

  shakadb_data_points_iterator_t it = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, SHAKADB_MIN_TIMESTAMP, SHAKADB_MAX_TIMESTAMP, &it);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status == 0, "Data found in iterator")

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_truncate_empty(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_points_iterator_t it_1 = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, SHAKADB_MIN_TIMESTAMP, SHAKADB_MAX_TIMESTAMP, &it_1);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it_1);
  sdb_assert(status == 0, "Data found in iterator")

  status = shakadb_truncate_data_series(&session, SDB_EUR_USD_ID);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when truncating");

  shakadb_data_points_iterator_t it_2 = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, SHAKADB_MIN_TIMESTAMP, SHAKADB_MAX_TIMESTAMP, &it_2);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

  status = shakadb_data_points_iterator_next(&it_2);
  sdb_assert(status == 0, "Data found in iterator")

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_truncate_and_write(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  for (int i = 0; i < 3; i++) {
    shakadb_data_point_t points[] = {
        {.time=1 + i, .value = 1 + i},
        {.time=2 + i, .value = 2 + i},
        {.time=3 + i, .value = 3 + i},
        {.time=4 + i, .value = 4 + i}
    };

    status = shakadb_write_points(&session, SDB_EUR_USD_ID, points, 4);
    sdb_assert(status == SHAKADB_RESULT_OK, "Error when sending");

    shakadb_data_points_iterator_t it = {};
    status = shakadb_read_points(&session, SDB_EUR_USD_ID, SHAKADB_MIN_TIMESTAMP, SHAKADB_MAX_TIMESTAMP, &it);
    sdb_assert(status == SHAKADB_RESULT_OK, "Unable to read data");

    status = shakadb_data_points_iterator_next(&it);
    sdb_assert(status != 0, "No data found in iterator")

    sdb_assert(it.points_count == 4, "Invalid number of points");
    sdb_assert(it.points[0].time == 1 + i, "Invalid time value");
    sdb_assert(it.points[1].time == 2 + i, "Invalid time value");
    sdb_assert(it.points[2].time == 3 + i, "Invalid time value");
    sdb_assert(it.points[3].time == 4 + i, "Invalid time value");
    sdb_assert(it.points[0].value == 1 + i, "Invalid value");
    sdb_assert(it.points[1].value == 2 + i, "Invalid value");
    sdb_assert(it.points[2].value == 3 + i, "Invalid value");
    sdb_assert(it.points[3].value == 4 + i, "Invalid value");

    status = shakadb_data_points_iterator_next(&it);
    sdb_assert(status == 0, "Data found in iterator")

    status = shakadb_truncate_data_series(&session, SDB_EUR_USD_ID);
    sdb_assert(status == SHAKADB_RESULT_OK, "Error when truncating");
  }

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_no_sig_pipe_on_too_large_packet(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t *points = (shakadb_data_point_t *)sdb_alloc(SDB_PACKET_MAX_LEN * sizeof(shakadb_data_point_t));

  status = shakadb_write_points(&session, SDB_EUR_GBP_ID, points, SDB_PACKET_MAX_LEN);
  sdb_assert(status == SHAKADB_RESULT_ERROR, "Packet should not be processed");

  shakadb_session_close(&session);

  sdb_free(points);
  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_failed_write(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create("/blah/blah", 10, SDB_DATA_SERIES_MAX);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4}
  };

  status = shakadb_write_points(&session, SDB_EUR_GBP_ID, points, 4);
  sdb_assert(status == SHAKADB_RESULT_ERROR, "Write should fail");

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_write_series_out_of_range(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, 10);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_point_t points[] = {
      {.time=1, .value = 1},
  };

  status = shakadb_write_points(&session, SDB_EUR_USD_ID, points, 1);
  sdb_assert(status != SHAKADB_RESULT_OK, "No Error when sending");

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_read_series_out_of_range(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, 10);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  shakadb_data_points_iterator_t it = {};
  status = shakadb_read_points(&session, SDB_EUR_USD_ID, SHAKADB_MIN_TIMESTAMP, SHAKADB_MAX_TIMESTAMP, &it);
  sdb_assert(status == SHAKADB_RESULT_OK, "Error when reading");

  status = shakadb_data_points_iterator_next(&it);
  sdb_assert(status == 0, "Data found in iterator")

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}

void sdb_test_server_truncate_series_out_of_range(sdb_tests_context_t ctx) {
  shakadb_session_t session;
  int status = 0;
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, 10);
  sdb_server_t *server = sdb_server_create(8081, 10, 10, 10, db);

  status = shakadb_session_open(&session, "localhost", 8081);
  sdb_assert(status == SHAKADB_RESULT_OK, "Unable to connect");

  status = shakadb_truncate_data_series(&session, SDB_EUR_USD_ID);
  sdb_assert(status != SHAKADB_RESULT_OK, "No Error when truncating");

  shakadb_session_close(&session);

  sdb_server_destroy(server);
  sdb_database_destroy(db);
}


// Write with invalid order
