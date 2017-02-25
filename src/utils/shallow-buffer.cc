//
// Created by Pawel Burzynski on 25/02/2017.
//

#include "shallow-buffer.h"

namespace shakadb {

ShallowBuffer::ShallowBuffer(byte_t *buffer, int size) {
  this->buffer = buffer;
  this->size = size;
}

byte_t *ShallowBuffer::GetBuffer() {
  return this->buffer;
}

int ShallowBuffer::GetSize() {
  return this->size;
}

}