//
// Created by Pawel Burzynski on 11/02/2017.
//

#include <cstdlib>
#include "ring-buffer.h"
#include "allocator.h"
#include "common.h"

namespace shakadb {

RingBuffer::RingBuffer(int grow_increment) {
  this->data = Allocator::New<uint8_t>(grow_increment);
  this->capacity = grow_increment;
  this->grow_increment = grow_increment;
  this->size = 0;
  this->begin = 0;
}

RingBuffer::~RingBuffer() {
  Allocator::Delete(this->data);
}

int RingBuffer::Read(uint8_t *buffer, int buffer_size) {
  int read_size = this->Peek(buffer, buffer_size);

  this->begin = (this->begin + read_size) % this->capacity;
  this->size -= read_size;

  return read_size;
}

int RingBuffer::Peek(uint8_t *buffer, int buffer_size) {
  int read_size = min(buffer_size, this->size);
  int read_end = (this->begin + read_size) % this->capacity;

  if (read_size == 0) {
    return 0;
  }

  if (this->begin < read_end) {
    memcpy(buffer, this->data + this->begin, read_size);
  } else {
    int read_first = this->capacity - this->begin;
    memcpy(buffer, this->data + this->begin, read_first);
    memcpy(buffer + read_first, this->data, read_end);
  }

  return read_size;
}

void RingBuffer::Write(uint8_t *buffer, int buffer_size) {
  if (buffer_size == 0) {
    return;
  }

  this->EnsureBufferSize(this->size + buffer_size);

  int data_end = (this->begin + this->size) % this->capacity;
  int write_end = (data_end + buffer_size) % this->capacity;

  if (data_end < write_end) {
    memcpy(this->data + data_end, buffer, buffer_size);
  } else {
    int write_first = this->capacity - data_end;
    memcpy(this->data + data_end, buffer, write_first);
    memcpy(this->data, buffer + write_first, write_end);
  }

  this->size += buffer_size;
}

int RingBuffer::GetSize() {
  return this->size;
}

bool RingBuffer::HasData(int size) {
  return size <= this->GetSize();
}

void RingBuffer::EnsureBufferSize(int new_size) {
  if (this->capacity >= new_size) {
    return;
  }

  int new_capacity = ((new_size / this->grow_increment) + 1) * this->grow_increment;
  uint8_t *new_data = Allocator::New<uint8_t>(new_capacity);
  this->Peek(new_data, this->size);

  Allocator::Delete(this->data);
  this->data = new_data;
  this->capacity = new_capacity;
  this->begin = 0;
}

}