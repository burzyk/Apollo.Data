//
// Created by Pawel Burzynski on 28/01/2017.
//

#ifndef SHAKADB_STORAGE_DATASERIES_H
#define SHAKADB_STORAGE_DATASERIES_H

#include <src/log.h>
#include <src/utils/rw-lock.h>
#include <list>
#include "data-point-reader.h"
#include "data-chunk.h"

namespace shakadb {

class DataSeries {
 public:
  ~DataSeries();
  static DataSeries *Init(std::string file_name, int points_per_chunk, Log *log);

  std::shared_ptr<DataPointReader> Read(timestamp_t begin, timestamp_t end);
  void Write(data_point_t *points, int count);

  void PrintMetadata();
 private:
  DataSeries(std::string file_name, int points_per_chunk, Log *log);
  void RegisterChunk(DataChunk *chunk);
  void WriteChunk(DataChunk *chunk, data_point_t *points, int count);
  void ChunkMemcpy(DataChunk *chunk, int position, data_point_t *points, int count);
  DataChunk *CreateEmptyChunk();

  Log *log;
  std::string file_name;
  int points_per_chunk;
  std::list<DataChunk *> chunks;
  RwLock series_lock;
};

}

#endif //SHAKADB_STORAGE_DATASERIES_H
