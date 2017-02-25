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
#include <src/utils/memory-buffer.h>
#include "data-chunk.h"
#include "src/data-points-reader.h"

namespace shakadb {

class StandardDataPointsReader : public DataPointsReader {
 public:
  StandardDataPointsReader(int points_count);

  void WriteDataPoints(data_point_t *points, int count);
  data_point_t *GetDataPoints();
  int GetDataPointsCount();
 private:
  MemoryBuffer points_buffer;
  int write_position;
};

}

#endif //SHAKADB_STORAGE_DATA_POINTS_READER_H