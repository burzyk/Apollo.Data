//
// Created by Pawel Burzynski on 25/02/2017.
//

#ifndef SHAKADB_BUFFER_H
#define SHAKADB_BUFFER_H

#include "common.h"

namespace shakadb {

class Buffer {
 public:
  virtual ~Buffer() {};
  virtual byte_t *GetBuffer() = 0;
  virtual int GetSize() = 0;
};

}

#endif //SHAKADB_BUFFER_H
