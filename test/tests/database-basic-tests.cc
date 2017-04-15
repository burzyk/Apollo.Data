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

#include "test/tests/database-basic-tests.h"

#include <memory>
#include <src/utils/memory.h>
#include <src/common.h>
#include <src/storage/database.h>
#include <src/storage/standard-database.h>

void sdb_test_database_write_with_time(shakadb::Database *db,
                                       sdb_data_series_id_t series_id,
                                       int batches,
                                       int count,
                                       sdb_timestamp_t time);
void sdb_test_database_write(shakadb::Database *db, sdb_data_series_id_t series_id, int batches, int count);
void sdb_test_database_validate_read_with_max(shakadb::Database *db,
                                              sdb_data_series_id_t series_id,
                                              int expected_count,
                                              sdb_timestamp_t begin,
                                              sdb_timestamp_t end,
                                              int max_points);
void sdb_test_database_validate_read(shakadb::Database *db,
                                     sdb_data_series_id_t series_id,
                                     int expected_count,
                                     sdb_timestamp_t begin,
                                     sdb_timestamp_t end);

void sdb_test_database_write(shakadb::Database *db, sdb_data_series_id_t series_id, int batches, int count) {
  sdb_test_database_write_with_time(db, series_id, batches, count, 1);
}

void sdb_test_database_validate_read(shakadb::Database *db,
                                     sdb_data_series_id_t series_id,
                                     int expected_count,
                                     sdb_timestamp_t begin,
                                     sdb_timestamp_t end) {
  sdb_test_database_validate_read_with_max(db, series_id, expected_count, begin, end, INT32_MAX);
}

void sdb_test_database_write_with_time(shakadb::Database *db,
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

    db->Write(series_id, points, count);
  }

  sdb_free(points);
}

void sdb_test_database_validate_read_with_max(shakadb::Database *db,
                                              sdb_data_series_id_t series_id,
                                              int expected_count,
                                              sdb_timestamp_t begin,
                                              sdb_timestamp_t end,
                                              int max_points) {
  sdb_data_points_reader_t *reader = db->Read(series_id, begin, end, max_points);
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

using namespace shakadb;

Database *CreateDatabase(int points_per_chunk, int unused, sdb_tests_context_t ctx) {
  return StandardDatabase::Init(ctx.working_directory, points_per_chunk);
}

void sdb_test_database_simple_initialization_test(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<shakadb::Database>(StandardDatabase::Init(ctx.working_directory, 5));
  db.reset();
}

void sdb_test_database_write_and_read_all(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 5, 3);
  sdb_test_database_validate_read(db.get(), 12345, 15, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_write_database_in_one_big_batch(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 1, 32);
  sdb_test_database_validate_read(db.get(), 12345, 32, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_write_database_in_multiple_small_batches(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 32, 1);
  sdb_test_database_validate_read(db.get(), 12345, 32, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_multi_write_and_read_all(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 100, 3);
  sdb_test_database_write(db.get(), 12345, 100, 3);
  sdb_test_database_write(db.get(), 12345, 100, 3);
  sdb_test_database_write(db.get(), 12345, 100, 3);
  sdb_test_database_validate_read(db.get(), 12345, 300, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_write_history(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 10000);
  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 1000);
  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 100);
  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 10);
  sdb_test_database_validate_read(db.get(), 12345, 60, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_write_close_and_write_more(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 5, 3);
  sdb_test_database_validate_read(db.get(), 12345, 15, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  db.reset();
  db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 100);
  sdb_test_database_validate_read(db.get(), 12345, 30, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  db.reset();
  db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 1000);
  sdb_test_database_validate_read(db.get(), 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_continuous_write(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 5, 3);
  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 30);
  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 100);
  sdb_test_database_validate_read(db.get(), 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_continuous_write_with_pickup(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 5, 3);
  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 30);
  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 100);
  sdb_test_database_validate_read(db.get(), 12345, 45, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  db.reset();
  db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 800);
  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 10000);
  sdb_test_database_write_with_time(db.get(), 12345, 5, 3, 100000);
  sdb_test_database_validate_read(db.get(), 12345, 90, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_write_batch_size_equal_to_page_capacity(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(5, 10, ctx));

  sdb_test_database_write(db.get(), 12345, 5, 5);
  sdb_test_database_write(db.get(), 12345, 5, 5);
  sdb_test_database_write(db.get(), 12345, 5, 5);
  sdb_test_database_write(db.get(), 12345, 5, 5);
  sdb_test_database_write(db.get(), 12345, 5, 5);
  sdb_test_database_write(db.get(), 12345, 5, 5);
  sdb_test_database_validate_read(db.get(), 12345, 25, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_write_batch_size_greater_than_page_capacity(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(5, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 100, 7);
  sdb_test_database_write(db.get(), 12345, 100, 7);
  sdb_test_database_validate_read(db.get(), 12345, 700, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_write_replace(sdb_tests_context_t ctx) {
  // throw ValidationException("Not implemented");
}

void sdb_test_database_read_inside_single_chunk(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(10, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 10, 10);
  sdb_test_database_validate_read(db.get(), 12345, 3, 2, 5);
}

void sdb_test_database_read_span_two_chunks(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(10, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 10, 10);
  sdb_test_database_validate_read(db.get(), 12345, 4, 8, 12);
}

void sdb_test_database_read_span_three_chunks(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(10, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 10, 10);
  sdb_test_database_validate_read(db.get(), 12345, 14, 8, 22);
}

void sdb_test_database_read_chunk_edges(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(10, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 10, 10);
  sdb_test_database_validate_read(db.get(), 12345, 10, 10, 20);
}

void sdb_test_database_read_duplicated_values(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(3, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 1, 2);
  sdb_test_database_write(db.get(), 12345, 1, 2);
  sdb_test_database_write(db.get(), 12345, 1, 2);
  sdb_test_database_write(db.get(), 12345, 1, 2);
  sdb_test_database_write(db.get(), 12345, 1, 2);
  sdb_test_database_validate_read(db.get(), 12345, 2, 0, 3);
  sdb_test_database_validate_read(db.get(), 12345, 1, 2, 3);
}

void sdb_test_database_read_with_limit(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(3, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 10, 10);
  sdb_test_database_validate_read_with_max(db.get(), 12345, 2, 0, 10, 2);
  sdb_test_database_validate_read_with_max(db.get(), 12345, 4, 0, 10, 4);
  sdb_test_database_validate_read_with_max(db.get(), 12345, 100, 0, 200, 200);
}

void sdb_test_database_truncate(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(3, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 1, 100);
  sdb_test_database_validate_read(db.get(), 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  db->Truncate(12345);

  sdb_test_database_validate_read(db.get(), 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_truncate_multiple(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(3, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 100, 1);
  sdb_test_database_validate_read(db.get(), 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  db->Truncate(12345);
  db->Truncate(1234555);
  db->Truncate(12345);

  sdb_test_database_validate_read(db.get(), 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

void sdb_test_database_truncate_write_again(sdb_tests_context_t ctx) {
  auto db = std::unique_ptr<Database>(CreateDatabase(3, 100, ctx));

  sdb_test_database_write(db.get(), 12345, 100, 1);
  sdb_test_database_validate_read(db.get(), 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);

  db->Truncate(12345);

  sdb_test_database_validate_read(db.get(), 12345, 0, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
  sdb_test_database_write(db.get(), 12345, 100, 1);
  sdb_test_database_validate_read(db.get(), 12345, 100, SDB_TIMESTAMP_MIN, SDB_TIMESTAMP_MAX);
}

