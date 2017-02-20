//
// Created by Pawel Burzynski on 11/02/2017.
//

#ifndef SHAKADB_STORAGE_RINGBUFFER_H
#define SHAKADB_STORAGE_RINGBUFFER_H

#include <cstdint>
#include "stream.h"

namespace shakadb {

class RingBuffer : public Stream {
 public:
  RingBuffer(int grow_increment);
  ~RingBuffer();

  int Read(byte_t *buffer, int buffer_size);
  int Peek(byte_t *buffer, int buffer_size);
  void Write(byte_t *buffer, int buffer_size);
  int GetSize();
  bool HasData(int size);
 private:
  void EnsureBufferSize(int new_size);

  byte_t *data;
  int size;
  int capacity;
  int begin;
  int grow_increment;
};

}

#endif //SHAKADB_STORAGE_RINGBUFFER_H
