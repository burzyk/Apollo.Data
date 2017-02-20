//
// Created by Pawel Burzynski on 16/02/2017.
//

#ifndef SHAKADB_STREAM_H
#define SHAKADB_STREAM_H

#include <cstdint>
#include "common.h"

namespace shakadb {

class Stream {
 public:
  virtual ~Stream() {};
  virtual int Read(byte_t *buffer, int buffer_size) = 0;
  virtual int Peek(byte_t *buffer, int buffer_size) = 0;
  virtual void Write(byte_t *buffer, int buffer_size) = 0;
  virtual bool HasData(int size) = 0;
};

}

#endif //SHAKADB_STREAM_H
