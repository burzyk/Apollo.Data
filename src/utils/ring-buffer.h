//
// Created by Pawel Burzynski on 11/02/2017.
//

#ifndef APOLLO_STORAGE_RINGBUFFER_H
#define APOLLO_STORAGE_RINGBUFFER_H

#include <cstdint>
namespace apollo {

class RingBuffer {
 public:
  RingBuffer(int initial_size);
  ~RingBuffer();

  void Read(uint8_t *buffer, int size);
  void Write(uint8_t *buffer, int size);
 private:
  uint8_t *data;
  int size;
  int begin;
  int end;
};

}

#endif //APOLLO_STORAGE_RINGBUFFER_H
