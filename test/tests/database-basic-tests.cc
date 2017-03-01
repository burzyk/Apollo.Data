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

namespace shakadb {
namespace test {

void DatabaseBasicTests::simple_database_initialization_test(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));
  db.reset();
}

void DatabaseBasicTests::basic_database_write_and_read_all(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 5, 3);
  ValidateRead(db.get(), "usd_gbp", 15, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void DatabaseBasicTests::write_database_in_one_big_batch(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 1, 32);
  ValidateRead(db.get(), "usd_gbp", 32, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void DatabaseBasicTests::write_database_in_multiple_small_batches(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 32, 1);
  ValidateRead(db.get(), "usd_gbp", 32, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void DatabaseBasicTests::database_multi_write_and_read_all(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 5, 3);
  Write(db.get(), "usd_gbp", 5, 3);
  Write(db.get(), "usd_gbp", 5, 3);
  Write(db.get(), "usd_gbp", 5, 3);
  ValidateRead(db.get(), "usd_gbp", 60, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void DatabaseBasicTests::database_write_history(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 5, 3, 10000);
  Write(db.get(), "usd_gbp", 5, 3, 1000);
  Write(db.get(), "usd_gbp", 5, 3, 100);
  Write(db.get(), "usd_gbp", 5, 3, 10);
  ValidateRead(db.get(), "usd_gbp", 60, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void DatabaseBasicTests::database_write_close_and_write_more(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 5, 3);
  ValidateRead(db.get(), "usd_gbp", 15, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);

  db.reset();
  db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 5, 3);
  ValidateRead(db.get(), "usd_gbp", 30, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);

  db.reset();
  db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 5, 3);
  ValidateRead(db.get(), "usd_gbp", 45, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void DatabaseBasicTests::database_continuous_write(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 5, 3);
  Write(db.get(), "usd_gbp", 5, 3, 30);
  Write(db.get(), "usd_gbp", 5, 3, 100);
  ValidateRead(db.get(), "usd_gbp", 45, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void DatabaseBasicTests::database_continuous_write_with_pickup(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 5, 3);
  Write(db.get(), "usd_gbp", 5, 3, 30);
  Write(db.get(), "usd_gbp", 5, 3, 100);
  ValidateRead(db.get(), "usd_gbp", 45, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);

  db.reset();
  db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 5, 3, 800);
  Write(db.get(), "usd_gbp", 5, 3, 10000);
  Write(db.get(), "usd_gbp", 5, 3, 100000);
  ValidateRead(db.get(), "usd_gbp", 90, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void DatabaseBasicTests::database_write_batch_size_equal_to_page_capacity(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 10, ctx));

  Write(db.get(), "usd_gbp", 5, 5);
  Write(db.get(), "usd_gbp", 5, 5);
  Write(db.get(), "usd_gbp", 5, 5);
  Write(db.get(), "usd_gbp", 5, 5);
  Write(db.get(), "usd_gbp", 5, 5);
  Write(db.get(), "usd_gbp", 5, 5);
  ValidateRead(db.get(), "usd_gbp", 150, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void DatabaseBasicTests::database_write_batch_size_greater_than_page_capacity(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(5, 100, ctx));

  Write(db.get(), "usd_gbp", 100, 7);
  Write(db.get(), "usd_gbp", 100, 7);
  ValidateRead(db.get(), "usd_gbp", 1400, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void DatabaseBasicTests::database_read_inside_single_chunk(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(10, 100, ctx));

  Write(db.get(), "usd_gbp", 10, 10);
  ValidateRead(db.get(), "usd_gbp", 3, 2, 5);
}

void DatabaseBasicTests::database_read_span_two_chunks(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(10, 100, ctx));

  Write(db.get(), "usd_gbp", 10, 10);
  ValidateRead(db.get(), "usd_gbp", 4, 8, 12);
}

void DatabaseBasicTests::database_read_span_three_chunks(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(10, 100, ctx));

  Write(db.get(), "usd_gbp", 10, 10);
  ValidateRead(db.get(), "usd_gbp", 14, 8, 22);
}

void DatabaseBasicTests::database_read_chunk_edges(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(10, 100, ctx));

  Write(db.get(), "usd_gbp", 10, 10);
  ValidateRead(db.get(), "usd_gbp", 10, 10, 20);
}

void DatabaseBasicTests::database_read_duplicated_values(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(3, 100, ctx));

  Write(db.get(), "usd_gbp", 1, 2);
  Write(db.get(), "usd_gbp", 1, 2);
  Write(db.get(), "usd_gbp", 1, 2);
  Write(db.get(), "usd_gbp", 1, 2);
  Write(db.get(), "usd_gbp", 1, 2);
  ValidateRead(db.get(), "usd_gbp", 5, 0, 2);
  ValidateRead(db.get(), "usd_gbp", 5, 2, 3);
}

void DatabaseBasicTests::database_read_with_limit(TestContext ctx) {
  auto db = std::unique_ptr<Database>(this->CreateDatabase(3, 100, ctx));

  Write(db.get(), "usd_gbp", 10, 10);
  ValidateRead(db.get(), "usd_gbp", 2, 0, 10, 2);
  ValidateRead(db.get(), "usd_gbp", 4, 0, 10, 4);
  ValidateRead(db.get(), "usd_gbp", 100, 0, 200, 200);
}

}  // namespace test
}  // namespace shakadb
