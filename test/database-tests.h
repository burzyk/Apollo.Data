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

class DatabaseContext {
 public:
  static DatabaseContext *Create(int page_size, int max_pages, TestContext ctx) {
    DatabaseContext *context = new DatabaseContext();
    context->master_lock = new RwLock();
    context->storage = CachedStorage::Init(ctx.GetWorkingDirectory() + "/DATA_FILE",
                                           Database::CalculatePageSize(page_size),
                                           max_pages);
    context->db = Database::Init(context->storage, context->master_lock);
    return context;
  }

  ~DatabaseContext() {
    delete this->db;
    delete this->storage;
    delete this->master_lock;
  }

  Database *GetDb() {
    return this->db;
  }
 private:
  DatabaseContext() {}

  Database *db;
  Storage *storage;
  RwLock *master_lock;
};

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

void validate_read(Database *db,
                   std::string series_name,
                   int expected_count,
                   timestamp_t begin,
                   timestamp_t end,
                   int buffer_size) {
  DataPointReader reader = db->Read(series_name, begin, end);
  int read = -1;
  uint64_t total_read = 0;
  data_point_t points[A_VALIDATE_READ_BUFFER_SIZE] = {0};
  data_point_t last = {0};

  while (read != 0) {
    read = reader.Read(points, buffer_size);
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

void validate_read(Database *db, std::string series_name, int expected_count, timestamp_t begin, timestamp_t end) {
  validate_read(db, series_name, expected_count, begin, end, A_VALIDATE_READ_BUFFER_SIZE);
}

void simple_database_initialization_test(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));
  c.release();
}

void basic_database_write_and_read_all(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  validate_read(c->GetDb(), "usd_gbp", 15, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
}

void write_database_in_one_big_batch(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 1, 32);
  validate_read(c->GetDb(), "usd_gbp", 32, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
}

void write_database_in_multiple_small_batches(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 32, 1);
  validate_read(c->GetDb(), "usd_gbp", 32, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
}

void database_multi_write_and_read_all(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  validate_read(c->GetDb(), "usd_gbp", 60, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
}

void database_write_history(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 10000);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 1000);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 100);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 10);
  validate_read(c->GetDb(), "usd_gbp", 60, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
}

void database_write_close_and_write_more(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  validate_read(c->GetDb(), "usd_gbp", 15, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  c.release();
  c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  validate_read(c->GetDb(), "usd_gbp", 30, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  c.release();
  c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  validate_read(c->GetDb(), "usd_gbp", 45, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
}

void database_continuous_write(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 30);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 100);
  validate_read(c->GetDb(), "usd_gbp", 45, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
}

void database_continuous_write_with_pickup(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 30);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 100);
  validate_read(c->GetDb(), "usd_gbp", 45, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);

  c.release();
  c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 800);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 10000);
  write_to_database(c->GetDb(), "usd_gbp", 5, 3, 100000);
  validate_read(c->GetDb(), "usd_gbp", 90, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
}

void database_write_batch_size_equal_to_page_capacity(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 10, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  write_to_database(c->GetDb(), "usd_gbp", 5, 5);
  validate_read(c->GetDb(), "usd_gbp", 150, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
}

void database_write_batch_size_greater_than_page_capacity(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(5, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 100, 7);
  write_to_database(c->GetDb(), "usd_gbp", 100, 7);
  validate_read(c->GetDb(), "usd_gbp", 1400, A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
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

void database_read_small_buffer(TestContext ctx) {
  auto c = std::unique_ptr<DatabaseContext>(DatabaseContext::Create(10, 100, ctx));

  write_to_database(c->GetDb(), "usd_gbp", 10, 10);
  validate_read(c->GetDb(), "usd_gbp", 10, 3, 13, 2);
  validate_read(c->GetDb(), "usd_gbp", 10, 3, 13, 7);
  validate_read(c->GetDb(), "usd_gbp", 10, 3, 13, 10);
}

}
}

