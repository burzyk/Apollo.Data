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
  shakadb_data_points_iterator_t iterator;
  shakadb_result_t result = shakadb_read_points(
      session,
      12345,
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

  while (shakadb_data_points_iterator_next(&iterator)) {
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

    shakadb_write_points(session, 12345, points, points_count);
  }

  printf("Elapsed: %f[s]\n", stopwatch_elapsed(sw));
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