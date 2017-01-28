//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_DATACHUNK_H
#define APOLLO_STORAGE_DATACHUNK_H

#include <string>
#include <src/data-point.h>
#include "src/engine/storage/storage-page.h"

namespace apollo {

#define A_MAX_DATA_SERIES_NAME  255

struct data_chunk_info_t {
  char series_name[A_MAX_DATA_SERIES_NAME];
};

class DataChunk {
 public:
  static DataChunk *Create(std::string series_name, StoragePage *page);
  static DataChunk *Load(StoragePage *page);

  int Read(int offset, data_point_t *points, int count);
  void Write(int offset, data_point_t *points, int count);

  std::string GetSeriesName();
  timestamp_t GetBegin();
  timestamp_t GetEnd();
  int GetNumberOfPoints();
  int GetMaxNumberOfPoints();

  void PrintMetadata();
 private:
  DataChunk(std::string series_name, StoragePage *page);
  std::shared_ptr<data_point_t> Read(int count);

  StoragePage *page;
  std::string series_name;
  timestamp_t begin;
  timestamp_t end;
  int number_of_points;
};

}

#endif //APOLLO_STORAGE_DATACHUNK_H
