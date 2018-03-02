//
// Created by Pawel Burzynski on 15/04/2017.
//

#include <pthread.h>
#include <unistd.h>

#include "src/network/server.h"
#include "src/network/client-handler.h"
#include "src/ctl/session.h"
#include "src/storage/database.h"
#include "test/server-tests.h"

#define SDB_EUR_USD_ID  123487
#define SDB_EUR_GBP_ID  323487

typedef struct float_data_point_s {
  timestamp_t time;
  float value;
} __attribute__((packed)) float_data_point_t;

typedef struct server_test_context_s {
  database_t *db;
  server_t *server;
  client_handler_t *handler;
  pthread_t runner;
  session_t *session;

} server_test_context_t;

void *server_test_routine(void *data);
int on_message_received(client_t *client, uint8_t *data, uint32_t size, void *context);
server_test_context_t *server_test_context_start(const char *directory, int max_series);
void server_test_context_stop(server_test_context_t *context);
int test_session_write(session_t *session, series_id_t series_id, float_data_point_t *points, uint64_t count);
float_data_point_t *get_points(session_t *session);

float_data_point_t *get_points(session_t *session) {
  return (float_data_point_t *)session->read_response->points;
}

int test_session_write(session_t *session, series_id_t series_id, float_data_point_t *points, uint64_t count) {
  points_list_t list = {
      .content = (data_point_t *)points,
      .point_size = 12,
      .count = count
  };
  return session_write(session, series_id, &list);
}

server_test_context_t *server_test_context_start(const char *directory, int max_series) {
  server_test_context_t *context = (server_test_context_t *)sdb_alloc(sizeof(server_test_context_t));
  context->db = database_create(directory, max_series);
  context->server = server_create(8081, on_message_received, context);
  pthread_create(&context->runner, NULL, server_test_routine, context->server);
  context->session = NULL;
  context->handler = client_handler_create(context->db);

  // wait until server is responsive
  while (context->session == NULL) {
    usleep(100);
    context->session = session_create("localhost", 8081);
  }

  return context;
}

int on_message_received(client_t *client, uint8_t *data, uint32_t size, void *context) {
  server_test_context_t *test_context = (server_test_context_t *)context;
  return client_handler_process_message(client, data, size, test_context->handler);
}

void *server_test_routine(void *data) {
  server_t *server = (server_t *)data;
  server_run(server);

  return NULL;
}

void server_test_context_stop(server_test_context_t *context) {
  if (context->session != NULL) {
    session_destroy(context->session);
  }

  server_stop(context->server);
  pthread_join(context->runner, NULL);
  server_destroy(context->server);
  client_handler_destroy(context->handler);
  database_destroy(context->db);
  sdb_free(context);
}

void test_server_simple_initialization_test(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);
  server_test_context_stop(context);
}

void test_server_connect(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  server_test_context_stop(context);
}

void test_server_connect_invalid_address(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  session_t *session = session_create("asdasdasdas.asdsa", 8081);
  sdb_assert(session == NULL, "Should not connect");

  server_test_context_stop(context);
}

void test_server_connect_invalid_port(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  session_t *session = session_create("localhost", 23445);
  sdb_assert(session == NULL, "Should not connect");

  server_test_context_stop(context);
}

void test_server_write_small(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  float_data_point_t points[] = {
      {.time=3, .value = 13},
      {.time=5, .value = 76},
      {.time=15, .value = 44}
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points, 3), "Error when sending");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");

  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 3, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 3, "Time should be 3");
  sdb_assert(get_points(context->session)[1].time == 5, "Time should be 5");
  sdb_assert(get_points(context->session)[2].time == 15, "Time should be 15");
  sdb_assert(get_points(context->session)[0].value == 13, "Value should be 13");
  sdb_assert(get_points(context->session)[1].value == 76, "Value should be 76");
  sdb_assert(get_points(context->session)[2].value == 44, "Value should be 44.3");

  sdb_assert(session_read_next(context->session) == 0, "Data left in iterator");

  server_test_context_stop(context);
}

void test_server_write_unordered(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  float_data_point_t points[] = {
      {.time=6, .value = 13},
      {.time=5, .value = 76},
      {.time=1, .value = 44},
      {.time=44, .value = 44}
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points, 4), "Error when sending");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");

  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 4, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 1, "Time should be 1");
  sdb_assert(get_points(context->session)[1].time == 5, "Time should be 5");
  sdb_assert(get_points(context->session)[2].time == 6, "Time should be 6");
  sdb_assert(get_points(context->session)[3].time == 44, "Time should be 44");
  sdb_assert(get_points(context->session)[0].value == 44, "Value should be 44");
  sdb_assert(get_points(context->session)[1].value == 76, "Value should be 76");
  sdb_assert(get_points(context->session)[2].value == 13, "Value should be 13");
  sdb_assert(get_points(context->session)[3].value == 44, "Value should be 44");

  sdb_assert(session_read_next(context->session) == 0, "Data left in iterator");

  server_test_context_stop(context);
}

void test_server_write_two_batches(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  float_data_point_t points_1[] = {
      {.time=1, .value = 1},
      {.time=4, .value = 2}
  };

  float_data_point_t points_2[] = {
      {.time=2, .value = 3},
      {.time=5, .value = 4}
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points_1, 2), "Error when sending");
  sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points_2, 2), "Error when sending");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");

  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 4, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 1, "Time should be 1");
  sdb_assert(get_points(context->session)[1].time == 2, "Time should be 2");
  sdb_assert(get_points(context->session)[2].time == 4, "Time should be 4");
  sdb_assert(get_points(context->session)[3].time == 5, "Time should be 5");
  sdb_assert(get_points(context->session)[0].value == 1, "Value should be 1");
  sdb_assert(get_points(context->session)[1].value == 3, "Value should be 3");
  sdb_assert(get_points(context->session)[2].value == 2, "Value should be 2");
  sdb_assert(get_points(context->session)[3].value == 4, "Value should be 4");

  sdb_assert(session_read_next(context->session) == 0, "Data left in iterator");

  server_test_context_stop(context);
}

void test_server_read_multiple_batches(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  uint64_t points_count = SDB_READ_MAX_PAYLOAD / sizeof(float_data_point_t);
  float_data_point_t *points = sdb_alloc(sizeof(float_data_point_t) * points_count);

  for (uint32_t i = 0; i < points_count; i++) {
    points[i].time = i + 1;
    points[i].value = i;
  }

  sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points, points_count), "Error when sending");

  for (uint32_t i = 0; i < points_count; i++) {
    points[i].time = i + points_count * 2;
    points[i].value = i;
  }

  sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points, points_count), "Error when sending");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");

  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");
  sdb_assert(context->session->read_response->points_count != 0, "Invalid number of points");

  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");
  sdb_assert(context->session->read_response->points_count != 0, "Invalid number of points");

  sdb_assert(session_read_next(context->session) == 0, "Data left in iterator");

  server_test_context_stop(context);
  sdb_free(points);
}

void test_server_read_range(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  float_data_point_t points[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4},
      {.time=5, .value = 5}
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points, 5), "Error when sending");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, 2, 4),
      "Unable to read data");

  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 2, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 2, "Time should be 2");
  sdb_assert(get_points(context->session)[1].time == 3, "Time should be 3");
  sdb_assert(get_points(context->session)[0].value == 2, "Value should be 2");
  sdb_assert(get_points(context->session)[1].value == 3, "Value should be 3");

  sdb_assert(session_read_next(context->session) == 0, "Data left in iterator");

  server_test_context_stop(context);
}

void test_server_read_range_with_multiple_series(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  float_data_point_t points_1[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4},
      {.time=5, .value = 5}
  };

  float_data_point_t points_2[] = {
      {.time=1, .value = 10},
      {.time=2, .value = 20},
      {.time=3, .value = 30},
      {.time=4, .value = 40},
      {.time=5, .value = 50}
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points_1, 5), "Error when sending");
  sdb_assert(!test_session_write(context->session, SDB_EUR_GBP_ID, points_2, 5), "Error when sending");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, 2, 4),
      "Unable to read data");

  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 2, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 2, "Time should be 2");
  sdb_assert(get_points(context->session)[1].time == 3, "Time should be 3");
  sdb_assert(get_points(context->session)[0].value == 2, "Value should be 2");
  sdb_assert(get_points(context->session)[1].value == 3, "Value should be 3");

  sdb_assert(session_read_next(context->session) == 0, "Data left in iterator");

  server_test_context_stop(context);
}

void test_server_update(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  float_data_point_t points_1[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4},
      {.time=5, .value = 5}
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points_1, 5), "Error when sending");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, 2, 4),
      "Unable to read data");

  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 2, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 2, "Time should be 2");
  sdb_assert(get_points(context->session)[1].time == 3, "Time should be 3");
  sdb_assert(get_points(context->session)[0].value == 2, "Value should be 2");
  sdb_assert(get_points(context->session)[1].value == 3, "Value should be 3");

  sdb_assert(session_read_next(context->session) == 0, "Data left in iterator");

  float_data_point_t points_2[] = {
      {.time=1, .value = 10},
      {.time=2, .value = 20},
      {.time=3, .value = 30},
      {.time=4, .value = 40},
      {.time=5, .value = 50}
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points_2, 5), "Error when sending");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, 2, 4),
      "Unable to read data");

  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 2, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 2, "Time should be 2");
  sdb_assert(get_points(context->session)[1].time == 3, "Time should be 3");
  sdb_assert(get_points(context->session)[0].value == 20, "Value should be 20");
  sdb_assert(get_points(context->session)[1].value == 30, "Value should be 30");

  sdb_assert(session_read_next(context->session) == 0, "Data left in iterator");

  server_test_context_stop(context);
}

void test_server_update_in_two_sessions(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  // First session
  session_t *session_1 = session_create("localhost", 8081);
  sdb_assert(session_1 != NULL, "Unable to connect");

  float_data_point_t points_1[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4},
      {.time=5, .value = 5}
  };

  sdb_assert(!test_session_write(session_1, SDB_EUR_USD_ID, points_1, 5), "Error when sending");

  sdb_assert(
      !session_read(session_1, SDB_EUR_USD_ID, 2, 4),
      "Unable to read data");

  sdb_assert(session_read_next(session_1) != 0, "No data found in iterator");

  sdb_assert(session_1->read_response->points_count == 2, "Invalid number of points");
  sdb_assert(get_points(session_1)[0].time == 2, "Time should be 2");
  sdb_assert(get_points(session_1)[1].time == 3, "Time should be 3");
  sdb_assert(get_points(session_1)[0].value == 2, "Value should be 2");
  sdb_assert(get_points(session_1)[1].value == 3, "Value should be 3");

  sdb_assert(session_read_next(session_1) == 0, "Data left in iterator");


  // Second session
  session_t *session_2 = session_create("localhost", 8081);
  sdb_assert(session_2 != NULL, "Unable to connect");

  float_data_point_t points_2[] = {
      {.time=1, .value = 10},
      {.time=2, .value = 20},
      {.time=3, .value = 30},
      {.time=4, .value = 40},
      {.time=5, .value = 50}
  };

  sdb_assert(!test_session_write(session_2, SDB_EUR_USD_ID, points_2, 5), "Error when sending");

  sdb_assert(
      !session_read(session_2, SDB_EUR_USD_ID, 2, 4),
      "Unable to read data");

  sdb_assert(session_read_next(session_2) != 0, "No data found in iterator");

  sdb_assert(session_2->read_response->points_count == 2, "Invalid number of points");
  sdb_assert(get_points(session_2)[0].time == 2, "Time should be 2");
  sdb_assert(get_points(session_2)[1].time == 3, "Time should be 3");
  sdb_assert(get_points(session_2)[0].value == 20, "Value should be 20");
  sdb_assert(get_points(session_2)[1].value == 30, "Value should be 30");

  sdb_assert(session_read_next(session_2) == 0, "Data left in iterator");

  session_destroy(session_1);
  session_destroy(session_2);

  server_test_context_stop(context);
}

void test_server_truncate_not_existing(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  sdb_assert(!session_truncate(context->session, SDB_EUR_USD_ID), "Error when truncating");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");
  sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

  server_test_context_stop(context);
}

void test_server_truncate_empty(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");
  sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

  sdb_assert(!session_truncate(context->session, SDB_EUR_USD_ID), "Error when truncating");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");
  sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

  server_test_context_stop(context);
}

void test_server_truncate_and_write(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  for (timestamp_t i = 0; i < 3; i++) {
    float_data_point_t points[] = {
        {.time=1 + i, .value = 1 + i},
        {.time=2 + i, .value = 2 + i},
        {.time=3 + i, .value = 3 + i},
        {.time=4 + i, .value = 4 + i}
    };

    sdb_assert(!test_session_write(context->session, SDB_EUR_USD_ID, points, 4), "Error when sending");

    sdb_assert(
        !session_read(context->session, SDB_EUR_USD_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
        "Unable to read data");
    sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

    sdb_assert(context->session->read_response->points_count == 4, "Invalid number of points");
    sdb_assert(get_points(context->session)[0].time == 1 + i, "Invalid time value");
    sdb_assert(get_points(context->session)[1].time == 2 + i, "Invalid time value");
    sdb_assert(get_points(context->session)[2].time == 3 + i, "Invalid time value");
    sdb_assert(get_points(context->session)[3].time == 4 + i, "Invalid time value");
    sdb_assert(get_points(context->session)[0].value == 1 + i, "Invalid value");
    sdb_assert(get_points(context->session)[1].value == 2 + i, "Invalid value");
    sdb_assert(get_points(context->session)[2].value == 3 + i, "Invalid value");
    sdb_assert(get_points(context->session)[3].value == 4 + i, "Invalid value");

    sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

    sdb_assert(!session_truncate(context->session, SDB_EUR_USD_ID), "Error when truncating");
  }

  server_test_context_stop(context);
}

void test_server_failed_write(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start("/blah/blah", SDB_DATA_SERIES_MAX);

  float_data_point_t points[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2},
      {.time=3, .value = 3},
      {.time=4, .value = 4}
  };

  sdb_assert(test_session_write(context->session, SDB_EUR_USD_ID, points, 4), "Write should fail");

  server_test_context_stop(context);
}

void test_server_write_series_out_of_range(test_context_t ctx) {
  server_test_context_t *context = server_test_context_start(ctx.working_directory, 10);

  float_data_point_t points[] = {
      {.time=1, .value = 1},
  };

  sdb_assert(test_session_write(context->session, SDB_EUR_USD_ID, points, 1), "Write should fail");

  server_test_context_stop(context);
}

void test_server_read_series_out_of_range(test_context_t ctx) {
  server_test_context_t *context = server_test_context_start(ctx.working_directory, 10);

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");
  sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

  server_test_context_stop(context);
}

void test_server_truncate_series_out_of_range(test_context_t ctx) {
  server_test_context_t *context = server_test_context_start(ctx.working_directory, 10);

  sdb_assert(!session_truncate(context->session, SDB_EUR_USD_ID), "Error when truncating");

  server_test_context_stop(context);
}

void test_server_write_filter_duplicates(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  float_data_point_t points[] = {
      {.time=1, .value = 1},
      {.time=1, .value = 2},
      {.time=2, .value = 3},
      {.time=3, .value = 4},
      {.time=4, .value = 5},
      {.time=4, .value = 6},
      {.time=4, .value = 7},
      {.time=4, .value = 8},
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_GBP_ID, points, 8), "Write failed");

  sdb_assert(
      !session_read(context->session, SDB_EUR_GBP_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");
  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 4, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 1, "Invalid time value");
  sdb_assert(get_points(context->session)[1].time == 2, "Invalid time value");
  sdb_assert(get_points(context->session)[2].time == 3, "Invalid time value");
  sdb_assert(get_points(context->session)[3].time == 4, "Invalid time value");

  sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

  server_test_context_stop(context);
}

void test_server_write_filter_zeros(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  float_data_point_t points[] = {
      {.time=1, .value = 1},
      {.time=1, .value = 2},
      {.time=2, .value = 3},
      {.time=0, .value = 10},
      {.time=3, .value = 4},
      {.time=4, .value = 5},
      {.time=4, .value = 6},
      {.time=0, .value = 11},
      {.time=4, .value = 7},
      {.time=4, .value = 8},
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_GBP_ID, points, 10), "Write failed");

  sdb_assert(
      !session_read(context->session, SDB_EUR_GBP_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");
  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 4, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 1, "Invalid time value");
  sdb_assert(get_points(context->session)[1].time == 2, "Invalid time value");
  sdb_assert(get_points(context->session)[2].time == 3, "Invalid time value");
  sdb_assert(get_points(context->session)[3].time == 4, "Invalid time value");

  sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

  server_test_context_stop(context);
}

void test_server_read_multiple_active(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  float_data_point_t points[] = {
      {.time=1, .value = 1},
      {.time=2, .value = 2}
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_GBP_ID, points, 2), "Write failed");

  // read first time
  sdb_assert(
      !session_read(context->session, SDB_EUR_GBP_ID, 1, 2),
      "Unable to read data");

  // read second time
  sdb_assert(
      session_read(context->session, SDB_EUR_GBP_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "There are multiple reads open");

  // initial data still returned
  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 1, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 1, "Invalid time value");

  sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

  // third read
  sdb_assert(
      !session_read(context->session, SDB_EUR_GBP_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");

  // third iterator returns data
  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points_count == 2, "Invalid number of points");
  sdb_assert(get_points(context->session)[0].time == 1, "Invalid time value");
  sdb_assert(get_points(context->session)[1].time == 2, "Invalid time value");

  sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

  server_test_context_stop(context);
}

void test_server_read_latest_series_out_of_range(test_context_t ctx) {
  server_test_context_t *context = server_test_context_start(ctx.working_directory, 10);

  sdb_assert(!session_read_latest(context->session, SDB_EUR_GBP_ID), "Read failed");
  sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

  server_test_context_stop(context);
}

void test_server_read_latest_when_empty(test_context_t ctx) {
  server_test_context_t *context = server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  sdb_assert(!session_read_latest(context->session, SDB_EUR_GBP_ID), "Read failed");
  sdb_assert(session_read_next(context->session) == 0, "Data found in iterator");

  server_test_context_stop(context);
}

void test_server_read_latest(test_context_t ctx) {
  server_test_context_t *context =
      server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);

  float_data_point_t points[] = {
      {.time=1, .value = 10},
      {.time=2, .value = 20},
      {.time=3, .value = 30},
      {.time=4, .value = 40},
      {.time=5, .value = 50},
  };

  sdb_assert(!test_session_write(context->session, SDB_EUR_GBP_ID, points, 5), "Write failed");

  sdb_assert(!session_read_latest(context->session, SDB_EUR_GBP_ID), "Read failed");
  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  sdb_assert(context->session->read_response->points[0].time == 5, "Invalid time");
  sdb_assert(get_points(context->session)[0].value == 50, "Invalid value");

  server_test_context_stop(context);
}

void test_server_write_varsize(test_context_t ctx) {
  server_test_context_t *context = server_test_context_start(ctx.working_directory, SDB_DATA_SERIES_MAX);
  data_point_t *curr = NULL;

  points_list_t request_list = {
      .point_size = 24,
      .count = 3,
      .content = sdb_alloc(24 * 3)
  };

  curr = request_list.content;

  curr->time = 3;
  memcpy(curr->value, "ala ma kota    ", 16);
  curr = data_point_next(&request_list, curr);

  curr->time = 5;
  memcpy(curr->value, "ola ma asa     ", 16);
  curr = data_point_next(&request_list, curr);

  curr->time = 7;
  memcpy(curr->value, "kotki na plotki", 16);

  sdb_assert(!session_write(context->session, SDB_EUR_USD_ID, &request_list), "Error when sending");

  sdb_assert(
      !session_read(context->session, SDB_EUR_USD_ID, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX),
      "Unable to read data");

  sdb_assert(session_read_next(context->session) != 0, "No data found in iterator");

  points_list_t response_list = {
      .content = context->session->read_response->points,
      .count = context->session->read_response->points_count,
      .point_size = context->session->read_response->point_size
  };

  sdb_assert(context->session->read_response->points_count == 3, "Invalid number of points");
  curr = response_list.content;

  sdb_assert(curr->time == 3, "Time should be 3");
  sdb_assert(!memcmp(curr->value, "ala ma kota    ", 16), "Value should be \"ala ma kota    \"");
  curr = data_point_next(&response_list, curr);

  sdb_assert(curr->time == 5, "Time should be 5");
  sdb_assert(!memcmp(curr->value, "ola ma asa     ", 16), "Value should be \"ola ma asa     \"");
  curr = data_point_next(&response_list, curr);

  sdb_assert(curr->time == 7, "Time should be 7");
  sdb_assert(!memcmp(curr->value, "kotki na plotki", 16), "Value should be \"kotki na plotki\"");
  curr = data_point_next(&response_list, curr);

  sdb_assert(session_read_next(context->session) == 0, "Data left in iterator");
  sdb_assert(curr == points_list_end(&response_list), "Data left in iterator");

  server_test_context_stop(context);
  sdb_free(request_list.content);
}
