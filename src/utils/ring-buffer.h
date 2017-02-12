//
// Created by Pawel Burzynski on 11/02/2017.
//

#ifndef APOLLO_STORAGE_RINGBUFFER_H
#define APOLLO_STORAGE_RINGBUFFER_H

#include <cstdint>
namespace apollo {

class RingBuffer {
 public:
  static const int kBufferGrowInvrement = 65536;

  RingBuffer();
  ~RingBuffer();

  int Read(uint8_t *buffer, int buffer_size);
  int Peek(uint8_t *buffer, int buffer_size);
  void Write(uint8_t *buffer, int buffer_size);
  int GetSize();
 private:
  void EnsureBufferSize(int new_size);

  uint8_t *data;
  int size;
  int capacity;
  int begin;
};

}

#endif //APOLLO_STORAGE_RINGBUFFER_H
