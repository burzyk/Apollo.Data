//
// Created by Pawel Burzynski on 25/02/2017.
//

#ifndef SHAKADB_SHALLOWBUFFER_H
#define SHAKADB_SHALLOWBUFFER_H

#include "buffer.h"

namespace shakadb {

class ShallowBuffer : public Buffer{
 public:
  ShallowBuffer(byte_t *buffer, int size);

  byte_t *GetBuffer();
  int GetSize();
 private:
  byte_t *buffer;
  int size;
};

}

#endif //SHAKADB_SHALLOWBUFFER_H
