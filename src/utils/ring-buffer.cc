/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//
// Created by Pawel Burzynski on 11/02/2017.
//

#include "src/utils/ring-buffer.h"

#include <string.h>
#include <cstdlib>
#include <algorithm>

#include "src/utils/allocator.h"
#include "src/utils/common.h"

namespace shakadb {

RingBuffer::RingBuffer(int grow_increment) {
  if (grow_increment <= 0) {
    FatalException("grow_increment must be greater than zero");
  }

  this->data = Allocator::New<byte_t>(grow_increment);
  this->capacity = grow_increment;
  this->grow_increment = grow_increment;
  this->size = 0;
  this->begin = 0;
}

RingBuffer::~RingBuffer() {
  Allocator::Delete(this->data);
}

int RingBuffer::Read(byte_t *buffer, int buffer_size) {
  int read_size = this->Peek(buffer, buffer_size);

  this->begin = (this->begin + read_size) % this->capacity;
  this->size -= read_size;

  return read_size;
}

int RingBuffer::Peek(byte_t *buffer, int buffer_size) {
  int read_size = std::min(buffer_size, this->size);
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

int RingBuffer::Write(byte_t *buffer, int buffer_size) {
  if (buffer_size == 0) {
    return 0;
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
  return buffer_size;
}

int RingBuffer::GetSize() {
  return this->size;
}

void RingBuffer::EnsureBufferSize(int new_size) {
  if (this->capacity >= new_size) {
    return;
  }

  int new_capacity =
      ((new_size / this->grow_increment) + 1) * this->grow_increment;
  byte_t *new_data = Allocator::New<byte_t>(new_capacity);
  this->Peek(new_data, this->size);

  Allocator::Delete(this->data);
  this->data = new_data;
  this->capacity = new_capacity;
  this->begin = 0;
}

}  // namespace shakadb
