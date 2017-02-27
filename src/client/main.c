//
// Created by Pawel Burzynski on 09/02/2017.
//

#include <stdio.h>
#include <time.h>
#include "client.h"

struct timespec stopwatch_start() {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return ts;
}

float stopwatch_elapsed(struct timespec start) {
  struct timespec stop;
  clock_gettime(CLOCK_REALTIME, &stop);

  uint64_t sec = stop.tv_sec - start.tv_sec;
  uint64_t nsec = stop.tv_nsec - start.tv_nsec;

  return (1000000000 * sec + nsec) / 1000000000.0f;
}

void read_points(shakadb_session_t *session) {
  shakadb_read_points_iterator_t iterator;
  shakadb_result_t result = shakadb_read_points(
      session,
      "USD_AUD",
      SHAKADB_MIN_TIMESTAMP,
      SHAKADB_MAX_TIMESTAMP,
      &iterator);

  if (result == SHAKADB_RESULT_ERROR) {
    printf("Unable to read data points\n");
    return;
  }

  shakadb_data_point_t curr = {0};
  shakadb_data_point_t prev = {0};
  uint64_t total_read = 0;
  struct timespec sw = stopwatch_start();

  while (shakadb_read_points_iterator_next(&iterator)) {
    for (int i = 0; i < iterator.points_count; i++) {
      curr = iterator.points[i];
      if (prev.time >= curr.time) {
        printf("Error reading prev: %llu, curr: %llu\n", prev.time, curr.time);
        return;
      }

//      printf("%llu -> %f\n", curr.time, curr.value);
      prev = curr;
      total_read++;
    }
  }

  printf("Total read points: %llu in %f[s]\n", total_read, stopwatch_elapsed(sw));
}

void write_points(shakadb_session_t *session) {
  int points_count = 1000;

  shakadb_data_point_t points[points_count];
  shakadb_timestamp_t time = 1;
  struct timespec sw = stopwatch_start();

  for (int i = 0; i < 100000; i++) {
    for (int j = 0; j < points_count; j++) {
      points[j].time = time++;
      points[j].value = j;
    }

    shakadb_write_points(session, "USD_AUD", points, points_count);
  }

  printf("Elapsed: %f[s]\n", stopwatch_elapsed(sw));
}

void ping_server(shakadb_session_t *session) {
  if (shakadb_ping(session) == SHAKADB_RESULT_ERROR) {
    printf("Ping failed\n");
  } else {
    printf("Ping failed\n");
  }
}

int main() {
  shakadb_session_t session;

  if (shakadb_open_session(&session, "localhost", 8099) == SHAKADB_RESULT_ERROR) {
    printf("Unable to connect\n");
    return -1;
  }

//  ping_server(&session);
  read_points(&session);
//  write_points(&session);

  shakadb_destroy_session(&session);

  return 0;
}