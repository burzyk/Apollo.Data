//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <src/utils/common.h>
#include <cstdlib>
#include <src/utils/allocator.h>
#include "standard-data-point-reader.h"

namespace shakadb {

StandardDataPointReader::StandardDataPointReader(int points_buffer_increment)
    : points_buffer(points_buffer_increment) {
  this->total_points = 0;
}

int StandardDataPointReader::ReadDataPoints(data_point_t *points, int count) {
  return this->points_buffer.Read((uint8_t *)points, count * sizeof(data_point_t)) / sizeof(data_point_t);
}

void StandardDataPointReader::WriteDataPoints(data_point_t *points, int count) {
  this->total_points += count;
  this->points_buffer.Write((uint8_t *)points, count * sizeof(data_point_t));
}

int StandardDataPointReader::GetDataPointsCount() {
  return this->total_points;
}

}
