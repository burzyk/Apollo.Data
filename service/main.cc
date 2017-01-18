#include <iostream>
#include <engine/storage/storage.h>
#include <engine/storage/cached-storage.h>
#include <engine/database.h>

int main() {
  // storage_allocate_page_test();
  // return 0;

  // Storage *storage = MmapFileStorage::Init("/Users/pburzynski/apollo-test/data/DATA_FILE", A_DATA_CHUNK_SIZE);
  apollo::Storage *storage = apollo::CachedStorage::Init(
      "/Users/pburzynski/apollo-test/data/DATA_FILE",
      sizeof(apollo::data_chunk_info_t) + 5 * sizeof(apollo::data_point_t));
  apollo::Database *db = apollo::Database::Init(storage);

  int batches = 3;
  int batch_size = 3;
  uint64_t time = 1;
  apollo::data_point_t *points = new apollo::data_point_t[batch_size];

  struct timespec start = {0};
  clock_gettime(CLOCK_REALTIME, &start);

  for (int i = 0; i < batches; i++) {
    for (int j = 0; j < batch_size; j++) {
      points[j].time = time++;
      points[j].value = time * 100;
    }

    db->Write("usd_gbp", points, batch_size);
  }

  struct timespec stop = {0};
  clock_gettime(CLOCK_REALTIME, &stop);
  printf("%lu.%lu\n", stop.tv_sec - start.tv_sec, stop.tv_nsec - start.tv_nsec);

  db->PrintMetadata();

  delete db;
  delete storage;

  return 0;
}