//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_DATACHUNK_H
#define APOLLO_STORAGE_DATACHUNK_H

#include <string>
#include <data-point.h>
#include "engine/storage/storage-page.h"

namespace apollo {

#define A_MAX_DATA_SERIES_NAME  255

struct data_chunk_info_t {
  char series_name[A_MAX_DATA_SERIES_NAME];
};

class DataChunk {
 public:
  static DataChunk *Create(std::string series_name, StoragePage *page);
  static DataChunk *Load(StoragePage *page);

  data_point_t *Read(uint64_t offset, uint64_t count);
  void Write(uint64_t offset, data_point_t *points, uint64_t count);

  std::string GetSeriesName();
  timestamp_t GetBegin();
  timestamp_t GetEnd();
  uint64_t GetNumberOfPoints();
  int GetMaxNumberOfPoints();

  void PrintMetadata();
 private:
  DataChunk(std::string series_name, StoragePage *page);

  StoragePage *page;
  std::string series_name;
  timestamp_t begin;
  timestamp_t end;
  uint64_t number_of_points;
};

}

#endif //APOLLO_STORAGE_DATACHUNK_H
