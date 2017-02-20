//
// Created by Pawel Burzynski on 15/02/2017.
//

#ifndef SHAKADB_DATA_POINT_READER_H
#define SHAKADB_DATA_POINT_READER_H

#include <src/data-point.h>

namespace shakadb {

class DataPointReader {
 public:
  virtual ~DataPointReader() {};

  virtual int ReadDataPoints(data_point_t *points, int count) = 0;
  virtual int GetDataPointsCount() = 0;
};

}

#endif //SHAKADB_DATA_POINT_READER_H
