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
// Created by Pawel Burzynski on 20/02/2017.
//

#include <list>
#include <src/utils/allocator.h>
#include <src/utils/common.h>
#include "read-points-iterator.h"

namespace shakadb {

ReadPointsIterator::ReadPointsIterator(Stream *response) {
  this->response = response;
  this->current = nullptr;
}

ReadPointsIterator::~ReadPointsIterator() {
  if (this->current != nullptr) {
    delete this->current;
  }
}

data_point_t *ReadPointsIterator::CurrentDataPoints() {
  return this->current == nullptr ? nullptr : this->current->GetPoints();
}

int ReadPointsIterator::CurrentDataPointsCount() {
  return this->current == nullptr ? -1 : this->current->GetPointsCount();
}

bool ReadPointsIterator::MoveNext() {
  if (this->current != nullptr) {
    delete this->current;
    this->current = nullptr;
  }

  DataPacket *packet = DataPacket::Load(this->response);

  if (packet == nullptr) {
    return false;
  }

  if (packet->GetType() != kReadResponse) {
    delete packet;
    return false;
  }

  ReadResponse *response = (ReadResponse *)packet;

  if (response->GetPointsCount() == 0) {
    delete response;
    return false;
  }

  this->current = response;
  return true;
}

}