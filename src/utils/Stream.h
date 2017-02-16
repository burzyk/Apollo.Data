//
// Created by Pawel Burzynski on 16/02/2017.
//

#ifndef SHAKADB_STREAM_H
#define SHAKADB_STREAM_H

#include <cstdint>

namespace shakadb {

class Stream {
 public:
  virtual ~Stream() {};
  virtual int Read(uint8_t *buffer, int buffer_size) = 0;
  virtual int Peek(uint8_t *buffer, int buffer_size) = 0;
  virtual void Write(uint8_t *buffer, int buffer_size) = 0;
  virtual int GetSize() = 0;
};

}

#endif //SHAKADB_STREAM_H
