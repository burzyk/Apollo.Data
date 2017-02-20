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
  this->position = 0;
}

StandardDataPointReader::~StandardDataPointReader() {
  if (this->snapshot != nullptr) {
    Allocator::Delete(this->snapshot);
    this->snapshot = nullptr;
  }

  this->count = 0;
}

int StandardDataPointReader::ReadDataPoints(data_point_t *points, int count) {
  int to_read = MIN(count, this->count - this->position);

  if (to_read == 0) {
    return 0;
  }

  memcpy(points, this->snapshot + this->position, to_read * sizeof(data_point_t));
  this->position += to_read;
  return to_read;
}

int StandardDataPointReader::GetDataPointsCount() {
  return this->count;
}

}
