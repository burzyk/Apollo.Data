//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <src/utils/common.h>
#include <cstdlib>
#include <src/utils/allocator.h>
#include "standard-data-points-reader.h"

namespace shakadb {

StandardDataPointsReader::StandardDataPointsReader(int points_count) {
  this->points_count = points_count;
  this->data_points = Allocator::New<data_point_t>(this->points_count);
  this->write_position = 0;
}

StandardDataPointsReader::~StandardDataPointsReader() {
  Allocator::Delete(this->data_points);
}

data_point_t *StandardDataPointsReader::GetDataPoints() {
  return this->data_points;
}

void StandardDataPointsReader::WriteDataPoints(data_point_t *points, int count) {
  int to_write = min(count, this->points_count - this->write_position);
  memcpy(this->data_points + this->write_position, points, to_write * sizeof(data_point_t));
  this->write_position += to_write;
}

int StandardDataPointsReader::GetDataPointsCount() {
  return this->points_count;
}

}
