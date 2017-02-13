//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <src/utils/common.h>
#include <cstdlib>
#include <src/utils/allocator.h>
#include "data-point-reader.h"

namespace shakadb {

DataPointReader::DataPointReader(data_point_t *snapshot, int count) {
  this->snapshot = snapshot;
  this->count = count;
}

DataPointReader::~DataPointReader() {
  if (this->snapshot != nullptr) {
    Allocator::Delete(this->snapshot);
    this->snapshot = nullptr;
  }

  this->count = 0;
}
data_point_t *DataPointReader::GetDataPoints() {
  return this->snapshot;
}

int DataPointReader::GetDataPointsCount() {
  return this->count;
}

}
