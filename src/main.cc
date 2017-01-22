#include <iostream>
#include <src/engine/storage/storage.h>
#include <src/engine/storage/cached-storage.h>
#include <src/engine/database.h>
#include <src/utils/stopwatch.h>


int main() {
//  apollo::Storage *storage = apollo::CachedStorage::Init(
//      "/Users/pburzynski/apollo-test/data/DATA_FILE",
//      apollo::Database::CalculatePageSize(1000000), 100);
//  apollo::Database *db = apollo::Database::Init(storage);
//
//  int batches = 1000000;
//  int batch_size = 100;
//  uint64_t time = 0;
//  apollo::data_point_t *points = new apollo::data_point_t[batch_size];

//  apollo::Stopwatch sw;
//  sw.Start();
//
//  for (int i = 0; i < batches; i++) {
//    for (int j = 0; j < batch_size; j++) {
//      time++;
//      points[j].time = time;
//      points[j].value = time * 100;
//    }
//
//    db->Write("usd_gbp", points, batch_size);
//  }
//
//  sw.Stop();
//  printf("Elapsed: %f / %d\n", sw.GetElapsedMilliseconds(), time);


//  apollo::Stopwatch sw_read;
//  sw_read.Start();
//
//  int begin = 0;
//  int read_batched = 1000000;
//  int total_read = 0;
//  int read = 0;
//
//  for (int i = 0; i < read_batched; i++) {
//    apollo::DataPointReader r = db->Read("usd_gbp", begin, begin + batch_size);
//
//    read = r.Read(points, batch_size);
//
//    for (int j = 0; j < read; j++) {
//      points[j].time++;
//      total_read++;
//    }
//
//    begin += batch_size;
//  }
//
//  sw_read.Stop();
//  printf("Elapsed: %f (%d)\n", sw_read.GetElapsedMilliseconds(), total_read);
//
//  delete db;
//  delete storage;

  return 0;
}