//
// Created by Pawel Burzynski on 25/02/2017.
//

#ifndef SHAKADB_MEMORYBUFFER_H
#define SHAKADB_MEMORYBUFFER_H

#include "buffer.h"

namespace shakadb {

class MemoryBuffer : public Buffer {
 public:
  MemoryBuffer(int size);
  ~MemoryBuffer();

  byte_t *GetBuffer();
  int GetSize();
 private:
  byte_t *buffer;
  int size;
};

}

#endif //SHAKADB_MEMORYBUFFER_H
