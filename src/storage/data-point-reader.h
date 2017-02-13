//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef SHAKADB_STORAGE_DATA_POINT_READER_H
#define SHAKADB_STORAGE_DATA_POINT_READER_H

#include <src/data-point.h>
#include <list>
#include <vector>
#include <src/utils/rw-lock-scope.h>
#include "data-chunk.h"

namespace shakadb {

class DataPointReader {
 public:
  DataPointReader(data_point_t *snapshot, int count);
  ~DataPointReader();

  data_point_t *GetDataPoints();
  int GetDataPointsCount();
 private:
  data_point_t *snapshot;
  int count;
};

}

#endif //SHAKADB_STORAGE_DATAPOINTREADER_H
