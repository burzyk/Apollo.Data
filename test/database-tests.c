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
// Created by Pawel Burzynski on 19/01/2017.
//

#include "database-tests.h"

#include "src/storage/database.h"

void test_database_write_with_time(database_t *db, series_id_t series_id, int batches, int count, timestamp_t time);
void test_database_write(database_t *db, series_id_t series_id, int batches, int count);
void test_database_validate_read_with_max(database_t *db,
                                          series_id_t series_id,
                                          int expected_count,
                                          timestamp_t begin,
                                          timestamp_t end,
                                          int max_points);
void test_database_validate_read(database_t *db,
                                 series_id_t series_id,
                                 int expected_count,
                                 timestamp_t begin,
                                 timestamp_t end);

void test_database_write(database_t *db, series_id_t series_id, int batches, int count) {
  test_database_write_with_time(db, series_id, batches, count, 1);
}

void test_database_validate_read(database_t *db,
                                 series_id_t series_id,
                                 int expected_count,
                                 timestamp_t begin,
                                 timestamp_t end) {
  test_database_validate_read_with_max(db, series_id, expected_count, begin, end, INT32_MAX);
}

void test_database_write_with_time(database_t *db,
                                   series_id_t series_id,
                                   int batches,
                                   int count,
                                   timestamp_t time) {
  sdb_assert(time != 0, "Time cannot be 0");

  data_point_t *points = (data_point_t *)sdb_alloc(sizeof(data_point_t) * count);

  for (int i = 0; i < batches; i++) {
    for (int j = 0; j < count; j++) {
      points[j].time = time;
      points[j].value = time * 100;
      time++;
    }

    database_write(db, series_id, points, count);
  }

  sdb_free(points);
}

void test_database_validate_read_with_max(database_t *db,
                                          series_id_t series_id,
                                          int expected_count,
                                          timestamp_t begin,
                                          timestamp_t end,
                                          int max_points) {
  points_reader_t *reader = database_read(db, series_id, begin, end, max_points);
  uint64_t total_read = reader->points.count;
  data_point_t *points = reader->points.content;

  for (int i = 1; i < total_read; i++) {
    sdb_assert(points[i - 1].time <= points[i].time, "Invalid order of elements");
    sdb_assert(points[i].time != 0, "Time cannot be zero");
  }

  if (expected_count > 0) {
    sdb_assert(expected_count == total_read, "Unexpected number of elements");
  }

  points_reader_destroy(reader);
}

void test_database_simple_initialization_test(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);
  database_destroy(db);
}

void test_database_write_and_read_all(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 3);
  test_database_validate_read(db, 12345, 15, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_write_database_in_one_big_batch(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 1, 32);
  test_database_validate_read(db, 12345, 32, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_write_database_in_multiple_small_batches(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 32, 1);
  test_database_validate_read(db, 12345, 32, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_multi_write_and_read_all(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 100, 3);
  test_database_write(db, 12345, 100, 3);
  test_database_write(db, 12345, 100, 3);
  test_database_write(db, 12345, 100, 3);
  test_database_validate_read(db, 12345, 300, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_write_history(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write_with_time(db, 12345, 5, 3, 10000);
  test_database_write_with_time(db, 12345, 5, 3, 1000);
  test_database_write_with_time(db, 12345, 5, 3, 100);
  test_database_write_with_time(db, 12345, 5, 3, 10);
  test_database_validate_read(db, 12345, 60, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_write_close_and_write_more(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 3);
  test_database_validate_read(db, 12345, 15, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);

  db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write_with_time(db, 12345, 5, 3, 100);
  test_database_validate_read(db, 12345, 30, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);

  db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write_with_time(db, 12345, 5, 3, 1000);
  test_database_validate_read(db, 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);
}

void test_database_continuous_write(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 3);
  test_database_write_with_time(db, 12345, 5, 3, 30);
  test_database_write_with_time(db, 12345, 5, 3, 100);
  test_database_validate_read(db, 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_continuous_write_with_pickup(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 3);
  test_database_write_with_time(db, 12345, 5, 3, 30);
  test_database_write_with_time(db, 12345, 5, 3, 100);
  test_database_validate_read(db, 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);

  db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write_with_time(db, 12345, 5, 3, 800);
  test_database_write_with_time(db, 12345, 5, 3, 10000);
  test_database_write_with_time(db, 12345, 5, 3, 100000);
  test_database_validate_read(db, 12345, 90, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);
}

void test_database_write_batch_size_equal_to_page_capacity(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 5);
  test_database_write(db, 12345, 5, 5);
  test_database_write(db, 12345, 5, 5);
  test_database_write(db, 12345, 5, 5);
  test_database_write(db, 12345, 5, 5);
  test_database_write(db, 12345, 5, 5);
  test_database_validate_read(db, 12345, 25, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_write_batch_size_greater_than_page_capacity(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 100, 7);
  test_database_write(db, 12345, 100, 7);
  test_database_validate_read(db, 12345, 700, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_read_inside_single_chunk(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 10, 10);
  test_database_validate_read(db, 12345, 3, 2, 5);

  database_destroy(db);
}

void test_database_read_span_two_chunks(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 10, 10);
  test_database_validate_read(db, 12345, 4, 8, 12);

  database_destroy(db);
}

void test_database_read_span_three_chunks(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 10, 10);
  test_database_validate_read(db, 12345, 14, 8, 22);

  database_destroy(db);
}

void test_database_read_chunk_edges(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 10, 10);
  test_database_validate_read(db, 12345, 10, 10, 20);

  database_destroy(db);
}

void test_database_read_duplicated_values(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 1, 2);
  test_database_write(db, 12345, 1, 2);
  test_database_write(db, 12345, 1, 2);
  test_database_write(db, 12345, 1, 2);
  test_database_write(db, 12345, 1, 2);
  test_database_validate_read(db, 12345, 2, 0, 3);
  test_database_validate_read(db, 12345, 1, 2, 3);

  database_destroy(db);
}

void test_database_read_with_limit(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 10, 10);
  test_database_validate_read_with_max(db, 12345, 2, 0, 10, 2);
  test_database_validate_read_with_max(db, 12345, 4, 0, 10, 4);
  test_database_validate_read_with_max(db, 12345, 100, 0, 200, 200);

  database_destroy(db);
}

void test_database_truncate(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 1, 100);
  test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_truncate(db, 12345);

  test_database_validate_read(db, 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_truncate_multiple(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 100, 1);
  test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_truncate(db, 12345);
  database_truncate(db, 1234555);
  database_truncate(db, 12345);

  test_database_validate_read(db, 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_truncate_write_again(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 100, 1);
  test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_truncate(db, 12345);

  test_database_validate_read(db, 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  test_database_write(db, 12345, 100, 1);
  test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_failed_write(test_context_t ctx) {
  database_t *db = database_create("/blah/blah", SDB_DATA_SERIES_MAX);
  data_point_t points[] = {{.time = 1, .value = 13}};

  int result = database_write(db, 12345, points, 1);
  sdb_assert(result != 0, "write operation should fail");

  database_destroy(db);
}

void test_database_read_latest_no_data(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  data_point_t latest = database_read_latest(db, 12345);

  sdb_assert(latest.value == 0, "Value is non-zero");
  sdb_assert(latest.time == 0, "Time is non-zero");

  database_destroy(db);
}

void test_database_read_latest_data_in_first_chunk(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 1, 10);
  data_point_t latest = database_read_latest(db, 12345);

  sdb_assert(latest.value == 1000, "Incorrect value");
  sdb_assert(latest.time == 10, "Incorrect time");

  database_destroy(db);
}

void test_database_read_latest_data_in_second_chunk(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 2, 10);
  data_point_t latest = database_read_latest(db, 12345);

  sdb_assert(latest.value == 2000, "Incorrect value");
  sdb_assert(latest.time == 20, "Incorrect time");

  database_destroy(db);
}
