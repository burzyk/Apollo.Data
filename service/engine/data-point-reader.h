//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_DATA_POINT_READER_H
#define APOLLO_STORAGE_DATA_POINT_READER_H

#include <data-point.h>

namespace apollo {

class DataPointReader {
 public:
  int Read(data_point_t *buffer, int size);
};

}

#endif //APOLLO_STORAGE_DATAPOINTREADER_H
