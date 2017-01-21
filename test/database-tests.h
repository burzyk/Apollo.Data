//
// Created by Pawel Burzynski on 19/01/2017.
//

#include <src/engine/database.h>
#include <src/engine/storage/cached-storage.h>
#include <memory>
#include <test/framework/assert.h>

namespace apollo {
namespace test {

#define A_VALIDATE_READ_BUFFER_SIZE 65536

void write_to_database(Database *db, std::string series_name, int batches, int batch_size, timestamp_t time) {
  apollo::data_point_t *points = new apollo::data_point_t[batch_size];

  for (int i = 0; i < batches; i++) {
    for (int j = 0; j < batch_size; j++) {
      points[j].time = time;
      points[j].value = time * 100;
      time++;
    }

    db->Write(series_name, points, batch_size);
  }

  delete[] points;
}

void write_to_database(Database *db, std::string series_name, int batches, int batch_size) {
  write_to_database(db, series_name, batches, batch_size, 1);
}

void validate_read(Database *db, std::string series_name, int expected_count, timestamp_t begin, timestamp_t end) {
  DataPointReader reader = db->Read(series_name, begin, end);
  int read = -1;
  uint64_t total_read = 0;
  data_point_t points[A_VALIDATE_READ_BUFFER_SIZE] = {0};
  data_point_t last = {0};

  while (read != 0) {
    read = reader.Read(points, A_VALIDATE_READ_BUFFER_SIZE);
    total_read += read;

    if (read == 0) {
      continue;
    }

    Assert::IsTrue(last.time <= points[0].time);

    for (int i = 1; i < read; i++) {
      Assert::IsTrue(points[i - 1].time <= points[i].time);
    }

    last = points[read - 1];
  }

  Assert::IsTrue(expected_count == total_read);
}

void simple_database_initialization_test(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  Database *db = Database::Init(storage);

  delete db;
  delete storage;
}

void basic_database_write_and_read_all(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  Database *db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 5, 3);
  validate_read(db, "usd_gbp", 15, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;

}

void write_database_in_one_big_batch(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  Database *db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 1, 32);
  validate_read(db, "usd_gbp", 32, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;
}

void write_database_in_multiple_small_batches(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  Database *db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 32, 1);
  validate_read(db, "usd_gbp", 32, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;
}

void database_multi_write_and_read_all(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  Database *db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 5, 3);
  write_to_database(db, "usd_gbp", 5, 3);
  write_to_database(db, "usd_gbp", 5, 3);
  write_to_database(db, "usd_gbp", 5, 3);
  validate_read(db, "usd_gbp", 60, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;
}

void database_write_close_and_write_more(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  Database *db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 5, 3);
  validate_read(db, "usd_gbp", 15, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;

  storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 5, 3);
  validate_read(db, "usd_gbp", 30, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;

  storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 5, 3);
  validate_read(db, "usd_gbp", 45, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;
}

void database_continuous_write(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  Database *db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 5, 3);
  write_to_database(db, "usd_gbp", 5, 3, 30);
  write_to_database(db, "usd_gbp", 5, 3, 100);
  validate_read(db, "usd_gbp", 45, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;
}

void database_continuous_write_with_pickup(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  Database *db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 5, 3);
  write_to_database(db, "usd_gbp", 5, 3, 30);
  write_to_database(db, "usd_gbp", 5, 3, 100);
  validate_read(db, "usd_gbp", 45, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;

  storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 5, 3, 800);
  write_to_database(db, "usd_gbp", 5, 3, 10000);
  write_to_database(db, "usd_gbp", 5, 3, 100000);
  validate_read(db, "usd_gbp", 90, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;
}

void database_write_batch_size_equal_to_page_capacity(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  Database *db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 5, 5);
  write_to_database(db, "usd_gbp", 5, 5);
  write_to_database(db, "usd_gbp", 5, 5);
  write_to_database(db, "usd_gbp", 5, 5);
  write_to_database(db, "usd_gbp", 5, 5);
  write_to_database(db, "usd_gbp", 5, 5);
  validate_read(db, "usd_gbp", 150, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;
}

void database_write_batch_size_greater_than_page_capacity(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(5));
  Database *db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 100, 7);
  write_to_database(db, "usd_gbp", 100, 7);
  validate_read(db, "usd_gbp", 1400, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  delete db;
  delete storage;
}

void database_read_inside_single_chunk(TestContext ctx) {
  CachedStorage *storage = CachedStorage::Init(
      ctx.GetWorkingDirectory() + "/DATA_FILE",
      Database::CalculatePageSize(10));
  Database *db = Database::Init(storage);

  write_to_database(db, "usd_gbp", 10, 10);
  validate_read(db, "usd_gbp", 3, 2, 5);

  delete db;
  delete storage;
}
}
}

