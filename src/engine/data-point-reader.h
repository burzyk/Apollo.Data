//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_DATA_POINT_READER_H
#define APOLLO_STORAGE_DATA_POINT_READER_H

#include <src/data-point.h>
#include <list>
#include <vector>
#include <src/utils/rw-lock-scope.h>
#include "data-chunk.h"

namespace apollo {

class DataPointReader {
 public:
  DataPointReader(std::list<DataChunk *> chunks,
                  timestamp_t begin,
                  timestamp_t end,
                  std::shared_ptr<RwLockScope> lock_scope);

  int Read(data_point_t *buffer, int size);
 private:
  std::vector<DataChunk *> chunks;
  std::vector<DataChunk *>::iterator current_chunk;
  timestamp_t begin;
  timestamp_t end;
  uint64_t position;
  std::shared_ptr<RwLockScope> lock_scope;
};

}

#endif //APOLLO_STORAGE_DATAPOINTREADER_H
