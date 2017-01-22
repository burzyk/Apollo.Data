//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_DATABASE_H
#define APOLLO_STORAGE_DATABASE_H

#include <string>
#include <list>
#include <map>
#include <src/utils/rw-lock.h>
#include <src/utils/log.h>
#include "src/engine/storage/storage.h"
#include "data-point-reader.h"
#include "data-chunk.h"

namespace apollo {

class Database {
 public:
  ~Database();
  static Database *Init(Storage *storage, RwLock *master_lock, Log *log);
  static int CalculatePageSize(int number_of_points);

  DataPointReader Read(std::string name, timestamp_t begin, timestamp_t end);
  void Write(std::string name, data_point_t *points, int count);

  void PrintMetadata();
 private:
  Database(Storage *storage, RwLock *master_lock, Log *log);
  void RegisterChunk(DataChunk *chunk);
  std::list<DataChunk *> *FindDataChunks(std::string name);
  void WriteChunk(DataChunk *chunk, data_point_t *points, int count);
  void ChunkMemcpy(DataChunk *chunk, int position, data_point_t *points, int count);

  Log *log;
  Storage *storage;
  uint64_t chunks_count;
  std::map<std::string, std::list<DataChunk *> *> series;
  RwLock *master_lock;
};

}

#endif //APOLLO_STORAGE_DATABASE_H
