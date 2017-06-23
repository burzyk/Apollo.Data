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

#include <memory.h>

#include "src/utils/memory.h"
#include "src/storage/database.h"

void sdb_test_database_write_with_time(sdb_database_t *db,
                                       sdb_data_series_id_t series_id,
                                       int batches,
                                       int count,
                                       sdb_timestamp_t time);
void sdb_test_database_write(sdb_database_t *db, sdb_data_series_id_t series_id, int batches, int count);
void sdb_test_database_validate_read_with_max(sdb_database_t *db,
                                              sdb_data_series_id_t series_id,
                                              int expected_count,
                                              sdb_timestamp_t begin,
                                              sdb_timestamp_t end,
                                              int max_points);
void sdb_test_database_validate_read(sdb_database_t *db,
                                     sdb_data_series_id_t series_id,
                                     int expected_count,
                                     sdb_timestamp_t begin,
                                     sdb_timestamp_t end);

void sdb_test_database_write(sdb_database_t *db, sdb_data_series_id_t series_id, int batches, int count) {
  sdb_test_database_write_with_time(db, series_id, batches, count, 1);
}

void sdb_test_database_validate_read(sdb_database_t *db,
                                     sdb_data_series_id_t series_id,
                                     int expected_count,
                                     sdb_timestamp_t begin,
                                     sdb_timestamp_t end) {
  sdb_test_database_validate_read_with_max(db, series_id, expected_count, begin, end, INT32_MAX);
}

void sdb_test_database_write_with_time(sdb_database_t *db,
                                       sdb_data_series_id_t series_id,
                                       int batches,
                                       int count,
                                       sdb_timestamp_t time) {
  sdb_assert(time != 0, "Time cannot be 0")

  sdb_data_point_t *points = (sdb_data_point_t *)sdb_alloc(sizeof(sdb_data_point_t) * count);

  for (int i = 0; i < batches; i++) {
    for (int j = 0; j < count; j++) {
      points[j].time = time;
      points[j].value = time * 100;
      time++;
    }

    sdb_database_write(db, series_id, points, count);
  }

  sdb_free(points);
}

void sdb_test_database_validate_read_with_max(sdb_database_t *db,
                                              sdb_data_series_id_t series_id,
                                              int expected_count,
                                              sdb_timestamp_t begin,
                                              sdb_timestamp_t end,
                                              int max_points) {
  sdb_data_points_reader_t *reader = sdb_database_read(db, series_id, begin, end, max_points);
  int total_read = reader->points_count;
  sdb_data_point_t *points = reader->points;

  for (int i = 1; i < total_read; i++) {
    sdb_assert(points[i - 1].time <= points[i].time, "Invalid order of elements")
    sdb_assert(points[i].time != 0, "Time cannot be zero")
  }

  if (expected_count > 0) {
    sdb_assert(expected_count == total_read, "Unexpected number of elements")
  }

  sdb_data_points_reader_destroy(reader);
}

void sdb_test_database_simple_initialization_test(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);
  sdb_database_destroy(db);
}

void sdb_test_database_write_and_read_all(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 5, 3);
  sdb_test_database_validate_read(db, 12345, 15, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_write_database_in_one_big_batch(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 1, 32);
  sdb_test_database_validate_read(db, 12345, 32, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_write_database_in_multiple_small_batches(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 32, 1);
  sdb_test_database_validate_read(db, 12345, 32, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_multi_write_and_read_all(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 100, 3);
  sdb_test_database_write(db, 12345, 100, 3);
  sdb_test_database_write(db, 12345, 100, 3);
  sdb_test_database_write(db, 12345, 100, 3);
  sdb_test_database_validate_read(db, 12345, 300, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_write_history(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write_with_time(db, 12345, 5, 3, 10000);
  sdb_test_database_write_with_time(db, 12345, 5, 3, 1000);
  sdb_test_database_write_with_time(db, 12345, 5, 3, 100);
  sdb_test_database_write_with_time(db, 12345, 5, 3, 10);
  sdb_test_database_validate_read(db, 12345, 60, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_write_close_and_write_more(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 5, 3);
  sdb_test_database_validate_read(db, 12345, 15, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  sdb_database_destroy(db);

  db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write_with_time(db, 12345, 5, 3, 100);
  sdb_test_database_validate_read(db, 12345, 30, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  sdb_database_destroy(db);

  db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write_with_time(db, 12345, 5, 3, 1000);
  sdb_test_database_validate_read(db, 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  sdb_database_destroy(db);
}

void sdb_test_database_continuous_write(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 5, 3);
  sdb_test_database_write_with_time(db, 12345, 5, 3, 30);
  sdb_test_database_write_with_time(db, 12345, 5, 3, 100);
  sdb_test_database_validate_read(db, 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_continuous_write_with_pickup(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 5, 3);
  sdb_test_database_write_with_time(db, 12345, 5, 3, 30);
  sdb_test_database_write_with_time(db, 12345, 5, 3, 100);
  sdb_test_database_validate_read(db, 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  sdb_database_destroy(db);

  db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write_with_time(db, 12345, 5, 3, 800);
  sdb_test_database_write_with_time(db, 12345, 5, 3, 10000);
  sdb_test_database_write_with_time(db, 12345, 5, 3, 100000);
  sdb_test_database_validate_read(db, 12345, 90, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  sdb_database_destroy(db);
}

void sdb_test_database_write_batch_size_equal_to_page_capacity(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 5, 5);
  sdb_test_database_write(db, 12345, 5, 5);
  sdb_test_database_write(db, 12345, 5, 5);
  sdb_test_database_write(db, 12345, 5, 5);
  sdb_test_database_write(db, 12345, 5, 5);
  sdb_test_database_write(db, 12345, 5, 5);
  sdb_test_database_validate_read(db, 12345, 25, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_write_batch_size_greater_than_page_capacity(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 5, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 100, 7);
  sdb_test_database_write(db, 12345, 100, 7);
  sdb_test_database_validate_read(db, 12345, 700, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_read_inside_single_chunk(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 10, 10);
  sdb_test_database_validate_read(db, 12345, 3, 2, 5);

  sdb_database_destroy(db);
}

void sdb_test_database_read_span_two_chunks(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 10, 10);
  sdb_test_database_validate_read(db, 12345, 4, 8, 12);

  sdb_database_destroy(db);
}

void sdb_test_database_read_span_three_chunks(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 10, 10);
  sdb_test_database_validate_read(db, 12345, 14, 8, 22);

  sdb_database_destroy(db);
}

void sdb_test_database_read_chunk_edges(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 10, 10);
  sdb_test_database_validate_read(db, 12345, 10, 10, 20);

  sdb_database_destroy(db);
}

void sdb_test_database_read_duplicated_values(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 3, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 1, 2);
  sdb_test_database_write(db, 12345, 1, 2);
  sdb_test_database_write(db, 12345, 1, 2);
  sdb_test_database_write(db, 12345, 1, 2);
  sdb_test_database_write(db, 12345, 1, 2);
  sdb_test_database_validate_read(db, 12345, 2, 0, 3);
  sdb_test_database_validate_read(db, 12345, 1, 2, 3);

  sdb_database_destroy(db);
}

void sdb_test_database_read_with_limit(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 3, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 10, 10);
  sdb_test_database_validate_read_with_max(db, 12345, 2, 0, 10, 2);
  sdb_test_database_validate_read_with_max(db, 12345, 4, 0, 10, 4);
  sdb_test_database_validate_read_with_max(db, 12345, 100, 0, 200, 200);

  sdb_database_destroy(db);
}

void sdb_test_database_truncate(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 3, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 1, 100);
  sdb_test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_truncate(db, 12345);

  sdb_test_database_validate_read(db, 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_truncate_multiple(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 3, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 100, 1);
  sdb_test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_truncate(db, 12345);
  sdb_database_truncate(db, 1234555);
  sdb_database_truncate(db, 12345);

  sdb_test_database_validate_read(db, 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_truncate_write_again(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 3, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);

  sdb_test_database_write(db, 12345, 100, 1);
  sdb_test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_truncate(db, 12345);

  sdb_test_database_validate_read(db, 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  sdb_test_database_write(db, 12345, 100, 1);
  sdb_test_database_validate_read(db, 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}

void sdb_test_database_failed_write(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create("/blah/blah", 3, SDB_DATA_SERIES_MAX, UINT64_MAX, UINT64_MAX);
  sdb_data_point_t points[] = {{.time = 1, .value = 13}};

  int result = sdb_database_write(db, 12345, points, 1);
  sdb_assert(result != 0, "write operation should fail");

  sdb_database_destroy(db);
}

void sdb_test_database_cache_cleanup(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 3, SDB_DATA_SERIES_MAX, 100, 200);

  sdb_test_database_write(db, 12345, 10, 6);
  sdb_test_database_validate_read(db, 12345, 60, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_assert(db->_cache->_allocated == 144, "Memory has not been cleaned up")

  sdb_database_destroy(db);
}

void sdb_test_database_cache_cleanup_old(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 10, SDB_DATA_SERIES_MAX, 120, 120);

  sdb_test_database_write(db, 12345, 2, 10);

  for (int i = 0; i < 10; i++) {
    sdb_data_points_reader_t *reader = sdb_database_read(db, 12345, 1, 10, 100);
    sdb_data_points_reader_destroy(reader);
  }

  sdb_assert(((sdb_data_chunk_t *)db->_cache->_guard.next->consumer)->begin == 1, "Invalid cached page");

  for (int i = 0; i < 10; i++) {
    sdb_data_points_reader_t *reader = sdb_database_read(db, 12345, 11, 100, 100);
    sdb_data_points_reader_destroy(reader);
  }

  sdb_assert(((sdb_data_chunk_t *)db->_cache->_guard.next->consumer)->begin == 11, "Invalid cached page");

  sdb_database_destroy(db);
}

void sdb_test_database_cache_smaller_than_chunk(sdb_tests_context_t ctx) {
  sdb_database_t *db = sdb_database_create(ctx.working_directory, 100, SDB_DATA_SERIES_MAX, 1, 1);

  sdb_test_database_write(db, 12345, 10, 6);
  sdb_test_database_validate_read(db, 12345, 60, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  sdb_database_destroy(db);
}