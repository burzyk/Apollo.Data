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
// Created by Pawel Burzynski on 16/02/2017.
//

#include "src/protocol/write-request.h"

#include <string.h>
#include <algorithm>

#include "src/utils/memory-buffer.h"
#include "src/utils/shallow-buffer.h"

namespace shakadb {

WriteRequest::WriteRequest() : WriteRequest(SHAKADB_INVALID_SERIES_ID, nullptr, 0) {
}

WriteRequest::WriteRequest(data_series_id_t series_id, data_point_t *points, int points_count) {
  this->series_id = series_id;
  this->points = points;
  this->points_count = points_count;
}

PacketType WriteRequest::GetType() {
  return kWriteRequest;
}

int WriteRequest::GetPointsCount() {
  return this->points_count;
}

data_point_t *WriteRequest::GetPoints() {
  return this->points;
}

data_series_id_t WriteRequest::GetSeriesId() {
  return this->series_id;
}

bool WriteRequest::Deserialize(Buffer *payload) {
  if (payload->GetSize() < sizeof(write_request_t)) {
    return false;
  }

  write_request_t *request = reinterpret_cast<write_request_t *>(payload->GetBuffer());

  this->points_count = request->points_count;
  this->series_id = request->series_id;

  if (payload->GetSize() != sizeof(write_request_t) + this->points_count * sizeof(data_point_t)) {
    return false;
  }

  this->points = reinterpret_cast<data_point_t *>(payload->GetBuffer() + sizeof(write_request_t));

  std::sort(this->points, this->points + this->points_count, [](data_point_t a, data_point_t b) -> bool {
    return a.time < b.time;
  });

  return true;
}

std::vector<Buffer *> WriteRequest::Serialize() {
  MemoryBuffer *info_fragment = new MemoryBuffer(sizeof(write_request_t));
  write_request_t *request = reinterpret_cast<write_request_t *>(info_fragment->GetBuffer());

  request->points_count = this->points_count;
  request->series_id = this->series_id;

  ShallowBuffer *points_fragment = new ShallowBuffer(
      reinterpret_cast<byte_t *>(this->points),
      this->points_count * sizeof(data_point_t));

  return std::vector<Buffer *> {info_fragment, points_fragment};
}

}  // namespace shakadb
