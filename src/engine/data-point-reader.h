//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_DATA_POINT_READER_H
#define APOLLO_STORAGE_DATA_POINT_READER_H

#include <src/data-point.h>
#include <list>
#include <vector>
#include "data-chunk.h"

namespace apollo {

class DataPointReader {
 public:
  DataPointReader(std::list<DataChunk *> chunks, timestamp_t begin, timestamp_t end);

  int Read(data_point_t *buffer, int size);
 private:
  std::vector<DataChunk *> chunks;
  std::vector<DataChunk *>::iterator current_chunk;
  timestamp_t begin;
  timestamp_t end;
  uint64_t position;
};

}

#endif //APOLLO_STORAGE_DATAPOINTREADER_H
