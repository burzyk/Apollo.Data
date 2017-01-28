//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <src/utils/common.h>
#include <cstdlib>
#include "data-point-reader.h"

namespace apollo {

DataPointReader::DataPointReader(data_point_t *snapshot, int count) {
  this->snapshot = snapshot;
  this->count = count;
}

DataPointReader::~DataPointReader() {
  if (this->snapshot != nullptr) {
    free(this->snapshot);
    this->snapshot = nullptr;
  }
}
data_point_t *DataPointReader::GetDataPoints() {
  return this->snapshot;
}

int DataPointReader::GetDataPointsCount() {
  return this->count;
}

}
