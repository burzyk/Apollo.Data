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
// Created by Pawel Burzynski on 19/02/2017.
//

#include "read-response.h"

namespace shakadb {

ReadResponse::ReadResponse() : ReadResponse(nullptr, 0) {
}

ReadResponse::ReadResponse(DataPointsReader *reader, int front_discard) {
  this->reader = reader;
  this->points = nullptr;
  this->points_count = 0;

  if (reader != nullptr) {
    this->points = reader->GetDataPoints() + front_discard;
    this->points_count = max(0, reader->GetDataPointsCount() - front_discard);
  }
}

PacketType ReadResponse::GetType() {
  return kReadResponse;
}

int ReadResponse::GetPointsCount() {
  return this->points_count;
}

data_point_t *ReadResponse::GetPoints() {
  return this->points;
}

bool ReadResponse::Deserialize(Buffer *payload) {
  this->points = (data_point_t *)payload->GetBuffer();
  this->points_count = payload->GetSize() / sizeof(data_point_t);

  return true;
}

std::vector<Buffer *> ReadResponse::Serialize() {
  std::vector<Buffer *> result;

  if (this->points_count != 0) {
    ShallowBuffer *points_fragment = new ShallowBuffer(
        (byte_t *)this->points,
        this->points_count * sizeof(data_point_t));
    result.push_back(points_fragment);
  }

  return result;
}

}