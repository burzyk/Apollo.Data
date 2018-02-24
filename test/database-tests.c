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

#include "test/database-tests.h"

#include <string.h>

#include "src/storage/database.h"

void test_database_write_with_time(database_t *db,
                                   series_id_t series_id,
                                   uint64_t batches,
                                   uint64_t count,
                                   timestamp_t time,
                                   uint32_t point_size);
void test_database_write(database_t *db, series_id_t series_id, uint64_t batches, uint64_t count, uint32_t point_size);
void test_database_validate_read_with_max(database_t *db,
                                          series_id_t series_id,
                                          uint64_t expected_count,
                                          timestamp_t begin,
                                          timestamp_t end,
                                          uint64_t max_points);
void test_database_validate_read(database_t *db,
                                 series_id_t series_id,
                                 uint64_t expected_count,
                                 timestamp_t begin,
                                 timestamp_t end);

void test_database_write(database_t *db, series_id_t series_id, uint64_t batches, uint64_t count, uint32_t point_size) {
  test_database_write_with_time(db, series_id, batches, count, 1, point_size);
}

void test_database_validate_read(database_t *db,
                                 series_id_t series_id,
                                 uint64_t expected_count,
                                 timestamp_t begin,
                                 timestamp_t end) {
  test_database_validate_read_with_max(db, series_id, expected_count, begin, end, INT32_MAX);
}

void test_database_write_with_time(database_t *db,
                                   series_id_t series_id,
                                   uint64_t batches,
                                   uint64_t count,
                                   timestamp_t time,
                                   uint32_t point_size) {
  sdb_assert(time != 0, "Time cannot be 0");

  points_list_t list = {.content=(data_point_t *)sdb_alloc(point_size * count), .count=count, .point_size=point_size};

  for (int i = 0; i < batches; i++) {
    data_point_t *curr = list.content;
    data_point_t *end = points_list_end(&list);

    while (curr != end) {
      curr->time = time;
      float v = time * 100;
      memcpy(&curr->value, &v, sizeof(v));

      time++;
      curr = data_point_next(&list, curr);
    }

    sdb_assert(database_write(db, series_id, &list) == 0, "Failed to write");
  }

  sdb_free(list.content);
}

void test_database_validate_read_with_max(database_t *db,
                                          series_id_t series_id,
                                          uint64_t expected_count,
                                          timestamp_t begin,
                                          timestamp_t end,
                                          uint64_t max_points) {
  points_reader_t *reader = database_read(db, series_id, begin, end, max_points);

  data_point_t *p1 = reader->points.content;
  data_point_t *p2 = data_point_next(&reader->points, p1);
  data_point_t *list_end = points_list_end(&reader->points);

  while (p2 != list_end) {
    sdb_assert(p1->time <= p2->time, "Invalid order of elements");
    sdb_assert(p2->time != 0, "Time cannot be zero");

    p1 = data_point_next(&reader->points, p1);
    p2 = data_point_next(&reader->points, p2);
  }

  if (expected_count > 0) {
    sdb_assert(expected_count == reader->points.count, "Unexpected number of elements");
  }

  points_reader_destroy(reader);
}

void test_database_simple_initialization_test(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);
  database_destroy(db);
}

void test_database_write_and_read_all(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 3, ctx.point_size);
  test_database_validate_read(db, 12345, 15, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_write_database_in_one_big_batch(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 1, 32, ctx.point_size);
  test_database_validate_read(db, 12345, 32, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_write_database_in_multiple_small_batches(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 32, 1, ctx.point_size);
  test_database_validate_read(db, 12345, 32, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_multi_write_and_read_all(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 100, 3, ctx.point_size);
  test_database_write(db, 12345, 100, 3, ctx.point_size);
  test_database_write(db, 12345, 100, 3, ctx.point_size);
  test_database_write(db, 12345, 100, 3, ctx.point_size);
  test_database_validate_read(db, 12345, 300, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_write_history(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write_with_time(db, 12345, 5, 3, 10000, ctx.point_size);
  test_database_write_with_time(db, 12345, 5, 3, 1000, ctx.point_size);
  test_database_write_with_time(db, 12345, 5, 3, 100, ctx.point_size);
  test_database_write_with_time(db, 12345, 5, 3, 10, ctx.point_size);
  test_database_validate_read(db, 12345, 60, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_write_close_and_write_more(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 3, ctx.point_size);
  test_database_validate_read(db, 12345, 15, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);

  db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write_with_time(db, 12345, 5, 3, 100, ctx.point_size);
  test_database_validate_read(db, 12345, 30, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);

  db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write_with_time(db, 12345, 5, 3, 1000, ctx.point_size);
  test_database_validate_read(db, 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);
}

void test_database_continuous_write(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 3, ctx.point_size);
  test_database_write_with_time(db, 12345, 5, 3, 30, ctx.point_size);
  test_database_write_with_time(db, 12345, 5, 3, 100, ctx.point_size);
  test_database_validate_read(db, 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_continuous_write_with_pickup(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 3, ctx.point_size);
  test_database_write_with_time(db, 12345, 5, 3, 30, ctx.point_size);
  test_database_write_with_time(db, 12345, 5, 3, 100, ctx.point_size);
  test_database_validate_read(db, 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);

  db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write_with_time(db, 12345, 5, 3, 800, ctx.point_size);
  test_database_write_with_time(db, 12345, 5, 3, 10000, ctx.point_size);
  test_database_write_with_time(db, 12345, 5, 3, 100000, ctx.point_size);
  test_database_validate_read(db, 12345, 90, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);
}

void test_database_write_close_read(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 3, ctx.point_size);
  test_database_validate_read(db, 12345, 15, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);

  db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_validate_read(db, 12345, 15, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  database_destroy(db);
}

void test_database_write_batch_size_equal_to_page_capacity(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 5, 5, ctx.point_size);
  test_database_write(db, 12345, 5, 5, ctx.point_size);
  test_database_write(db, 12345, 5, 5, ctx.point_size);
  test_database_write(db, 12345, 5, 5, ctx.point_size);
  test_database_write(db, 12345, 5, 5, ctx.point_size);
  test_database_write(db, 12345, 5, 5, ctx.point_size);
  test_database_validate_read(db, 12345, 25, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_write_batch_size_greater_than_page_capacity(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 100, 7, ctx.point_size);
  test_database_write(db, 12345, 100, 7, ctx.point_size);
  test_database_validate_read(db, 12345, 700, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_read_inside_single_chunk(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 10, 10, ctx.point_size);
  test_database_validate_read(db, 12345, 3, 2, 5);

  database_destroy(db);
}

void test_database_read_span_two_chunks(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 10, 10, ctx.point_size);
  test_database_validate_read(db, 12345, 4, 8, 12);

  database_destroy(db);
}

void test_database_read_span_three_chunks(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 10, 10, ctx.point_size);
  test_database_validate_read(db, 12345, 14, 8, 22);

  database_destroy(db);
}

void test_database_read_chunk_edges(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 10, 10, ctx.point_size);
  test_database_validate_read(db, 12345, 10, 10, 20);

  database_destroy(db);
}

void test_database_read_duplicated_values(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 1, 2, ctx.point_size);
  test_database_write(db, 12345, 1, 2, ctx.point_size);
  test_database_write(db, 12345, 1, 2, ctx.point_size);
  test_database_write(db, 12345, 1, 2, ctx.point_size);
  test_database_write(db, 12345, 1, 2, ctx.point_size);
  test_database_validate_read(db, 12345, 2, 0, 3);
  test_database_validate_read(db, 12345, 1, 2, 3);

  database_destroy(db);
}

void test_database_read_with_limit(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 10, 10, ctx.point_size);
  test_database_validate_read_with_max(db, 12345, 2, 0, 10, 2);
  test_database_validate_read_with_max(db, 12345, 4, 0, 10, 4);
  test_database_validate_read_with_max(db, 12345, 100, 0, 200, 200);

  database_destroy(db);
}

void test_database_truncate(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 1, 100, ctx.point_size);
  test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_truncate(db, 12345);

  test_database_validate_read(db, 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_truncate_multiple(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 100, 1, ctx.point_size);
  test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_truncate(db, 12345);
  database_truncate(db, 1234555);
  database_truncate(db, 12345);

  test_database_validate_read(db, 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_truncate_write_again(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 100, 1, ctx.point_size);
  test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_truncate(db, 12345);

  test_database_validate_read(db, 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  test_database_write(db, 12345, 100, 1, ctx.point_size);
  test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  database_destroy(db);
}

void test_database_failed_write(test_context_t ctx) {
  database_t *db = database_create("/blah/blah", SDB_DATA_SERIES_MAX);
  data_point_t points[] = {{.time = 1, .value = 13}};

  points_list_t list = {.content=points, .point_size=12, .count=1};
  int result = database_write(db, 12345, &list);
  sdb_assert(result != 0, "write operation should fail");

  database_destroy(db);
}

void test_database_read_latest_no_data(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  data_point_t *latest = database_read_latest(db, 12345)->points.content;

  sdb_assert(latest == NULL, "Expected NULL value");

  database_destroy(db);
}

void test_database_read_latest(test_context_t ctx) {
  database_t *db = database_create(ctx.working_directory, SDB_DATA_SERIES_MAX);

  test_database_write(db, 12345, 1, 10, ctx.point_size);
  test_database_write(db, 12345, 1, 10, ctx.point_size);
  data_point_t latest_12 = database_read_latest(db, 12345)->points.content[0];
  data_point_t latest_16 = database_read_latest(db, 12345)->points.content[0];

  sdb_assert(latest_12.value == 10900, "Incorrect value");
  sdb_assert(latest_12.time == 109, "Incorrect time");

  sdb_assert(latest_16.value == 10900, "Incorrect value");
  sdb_assert(latest_16.time == 109, "Incorrect time");

  database_destroy(db);
}
