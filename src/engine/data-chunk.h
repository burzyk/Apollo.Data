//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_DATACHUNK_H
#define APOLLO_STORAGE_DATACHUNK_H

#include <string>
#include <src/data-point.h>
#include "src/engine/storage/storage-page.h"

namespace apollo {

class DataChunk {
 public:
  static DataChunk *Create(StoragePage *page);
  static DataChunk *Load(StoragePage *page);

  int Read(int offset, data_point_t *points, int count);
  void Write(int offset, data_point_t *points, int count);

  timestamp_t GetBegin();
  timestamp_t GetEnd();
  int GetNumberOfPoints();
  int GetMaxNumberOfPoints();

  void PrintMetadata();
 private:
  DataChunk(StoragePage *page);

  StoragePage *page;
  timestamp_t begin;
  timestamp_t end;
  int number_of_points;
};

}

#endif //APOLLO_STORAGE_DATACHUNK_H
