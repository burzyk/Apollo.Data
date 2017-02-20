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
#include "src/data-point-reader.h"

namespace shakadb {

class StandardDataPointReader : public DataPointReader {
 public:
  StandardDataPointReader(data_point_t *snapshot, int count);
  ~StandardDataPointReader();

  int ReadDataPoints(data_point_t *points, int count);
  int GetDataPointsCount();
 private:
  data_point_t *snapshot;
  int count;
  int position;
};

}

#endif //SHAKADB_STORAGE_DATAPOINTREADER_H