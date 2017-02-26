//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <string.h>
#include <src/utils/common.h>
#include <cstdlib>
#include <src/utils/allocator.h>
#include "standard-data-points-reader.h"

namespace shakadb {

StandardDataPointsReader::StandardDataPointsReader(int points_count)
    : points_buffer(points_count * sizeof(data_point_t)) {
  this->write_position = 0;
}

data_point_t *StandardDataPointsReader::GetDataPoints() {
  return (data_point_t *)this->points_buffer.GetBuffer();
}

bool StandardDataPointsReader::WriteDataPoints(data_point_t *points, int count) {
  if (count == 0) {
    return false;
  }

  int to_write = min(count, this->GetDataPointsCount() - this->write_position);
  memcpy(this->GetDataPoints() + this->write_position, points, to_write * sizeof(data_point_t));
  this->write_position += to_write;

  return this->write_position < this->GetDataPointsCount();
}

int StandardDataPointsReader::GetDataPointsCount() {
  return this->points_buffer.GetSize() / sizeof(data_point_t);
}

}
