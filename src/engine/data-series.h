//
// Created by Pawel Burzynski on 28/01/2017.
//

#ifndef APOLLO_STORAGE_DATASERIES_H
#define APOLLO_STORAGE_DATASERIES_H

#include <src/engine/storage/storage.h>
#include <src/utils/log.h>
#include "data-point-reader.h"

namespace apollo {

class DataSeries {
 public:
  ~DataSeries();
  static DataSeries *Init(Storage *storage, Log *log);

  DataPointReader Read(timestamp_t begin, timestamp_t end);
  void Write(data_point_t *points, int count);

  void PrintMetadata();
 private:
  DataSeries(Storage *storage, Log *log);
  void RegisterChunk(DataChunk *chunk);
  void WriteChunk(DataChunk *chunk, data_point_t *points, int count);
  void ChunkMemcpy(DataChunk *chunk, int position, data_point_t *points, int count);

  Log *log;
  Storage *storage;
  std::list<DataChunk *> chunks;
};

}

#endif //APOLLO_STORAGE_DATASERIES_H
