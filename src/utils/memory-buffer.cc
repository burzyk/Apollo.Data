//
// Created by Pawel Burzynski on 25/02/2017.
//

#include "memory-buffer.h"
#include "allocator.h"

namespace shakadb {

MemoryBuffer::MemoryBuffer(int size) {
  this->size = size;
  this->buffer = this->size == 0 ? nullptr : Allocator::New<byte_t>(this->size);
}

MemoryBuffer::~MemoryBuffer() {
  if (this->buffer != nullptr) {
    Allocator::Delete(this->buffer);
  }
}

byte_t *MemoryBuffer::GetBuffer() {
  return this->buffer;
}

int MemoryBuffer::GetSize() {
  return this->size;
}

}