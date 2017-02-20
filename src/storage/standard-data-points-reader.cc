//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <src/utils/common.h>
#include <cstdlib>
#include <src/utils/allocator.h>
#include "standard-data-points-reader.h"

namespace shakadb {

StandardDataPointsReader::StandardDataPointsReader(int points_buffer_increment)
    : points_buffer(points_buffer_increment) {
  this->total_points = 0;
}

int StandardDataPointsReader::ReadDataPoints(data_point_t *points, int count) {
  return this->points_buffer.Read((byte_t *)points, count * sizeof(data_point_t)) / sizeof(data_point_t);
}

void StandardDataPointsReader::WriteDataPoints(data_point_t *points, int count) {
  this->total_points += count;
  this->points_buffer.Write((byte_t *)points, count * sizeof(data_point_t));
}

int StandardDataPointsReader::GetDataPointsCount() {
  return this->total_points;
}

}
