//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef SHAKADB_STORAGE_DATA_POINTS_READER_H
#define SHAKADB_STORAGE_DATA_POINTS_READER_H

#include <src/data-point.h>
#include <list>
#include <vector>
#include <src/utils/rw-lock-scope.h>
#include <src/utils/ring-buffer.h>
#include "data-chunk.h"
#include "src/data-points-reader.h"

namespace shakadb {

class StandardDataPointsReader : public DataPointsReader {
 public:
  StandardDataPointsReader(int points_buffer_increment);

  int ReadDataPoints(data_point_t *points, int count);
  void WriteDataPoints(data_point_t *points, int count);
  int GetDataPointsCount();
 private:
  RingBuffer points_buffer;
  int total_points;
};

}

#endif //SHAKADB_STORAGE_DATA_POINTS_READER_H
