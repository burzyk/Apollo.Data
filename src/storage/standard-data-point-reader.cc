//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <src/utils/common.h>
#include <cstdlib>
#include <src/utils/allocator.h>
#include "standard-data-point-reader.h"

namespace shakadb {

StandardDataPointReader::StandardDataPointReader(data_point_t *snapshot, int count) {
  this->snapshot = snapshot;
  this->count = count;
}

StandardDataPointReader::~StandardDataPointReader() {
  if (this->snapshot != nullptr) {
    Allocator::Delete(this->snapshot);
    this->snapshot = nullptr;
  }

  this->count = 0;
}
data_point_t *StandardDataPointReader::GetDataPoints() {
  return this->snapshot;
}

int StandardDataPointReader::GetDataPointsCount() {
  return this->count;
}

}
