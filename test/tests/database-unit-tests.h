//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef SHAKADB_STORAGE_ENGINE_UNIT_TESTS_H
#define SHAKADB_STORAGE_ENGINE_UNIT_TESTS_H

#include <src/database.h>
#include <memory>
#include <test/framework/assert.h>
#include <src/file-log.h>
#include <src/utils/stopwatch.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "database-common.h"

namespace shakadb {
namespace test {

void simple_database_initialization_test(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));
  c.release();
}

void basic_database_write_and_read_all(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  validate_read(c->GetDb(), "usd_gbp", 15, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void write_database_in_one_big_batch(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 1, 32);
  validate_read(c->GetDb(), "usd_gbp", 32, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void write_database_in_multiple_small_batches(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 32, 1);
  validate_read(c->GetDb(), "usd_gbp", 32, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void database_multi_write_and_read_all(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  validate_read(c->GetDb(), "usd_gbp", 60, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void database_write_history(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 10000);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 1000);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 100);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 10);
  validate_read(c->GetDb(), "usd_gbp", 60, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void database_write_close_and_write_more(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  validate_read(c->GetDb(), "usd_gbp", 15, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);

  c.reset();
  c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  validate_read(c->GetDb(), "usd_gbp", 30, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);

  c.reset();
  c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  validate_read(c->GetDb(), "usd_gbp", 45, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void database_continuous_write(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 30);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 100);
  validate_read(c->GetDb(), "usd_gbp", 45, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void database_continuous_write_with_pickup(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 30);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 100);
  validate_read(c->GetDb(), "usd_gbp", 45, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);

  c.reset();
  c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 800);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 10000);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 100000);
  validate_read(c->GetDb(), "usd_gbp", 90, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void database_write_batch_size_equal_to_page_capacity(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 10, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  validate_read(c->GetDb(), "usd_gbp", 150, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void database_write_batch_size_greater_than_page_capacity(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 100, 7);
  write_to_database(c->GetDb(), "usd_gbp", 100, 7);
  validate_read(c->GetDb(), "usd_gbp", 1400, data_point_t::kMinTimestamp, data_point_t::kMaxTimestamp);
}

void database_read_inside_single_chunk(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(10, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 10, 10);
  validate_read(c->GetDb(), "usd_gbp", 3, 2, 5);
}

void database_read_span_two_chunks(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(10, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 10, 10);
  validate_read(c->GetDb(), "usd_gbp", 4, 8, 12);
}

void database_read_span_three_chunks(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(10, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 10, 10);
  validate_read(c->GetDb(), "usd_gbp", 14, 8, 22);
}

void database_read_chunk_edges(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(10, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 10, 10);
  validate_read(c->GetDb(), "usd_gbp", 10, 10, 20);
}

void database_read_duplicated_values(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(3, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 1, 2);
  write_to_database(c->GetDb(), "usd_gbp", 1, 2);
  write_to_database(c->GetDb(), "usd_gbp", 1, 2);
  write_to_database(c->GetDb(), "usd_gbp", 1, 2);
  write_to_database(c->GetDb(), "usd_gbp", 1, 2);
  validate_read(c->GetDb(), "usd_gbp", 5, 0, 2);
  validate_read(c->GetDb(), "usd_gbp", 5, 2, 3);
}

}
}

#endif
