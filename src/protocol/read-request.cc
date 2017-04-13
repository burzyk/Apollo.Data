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

#include "src/protocol/read-request.h"

#include <string.h>

#include "src/utils/memory-buffer.h"
#include "src/fatal-exception.h"

namespace shakadb {

ReadRequest::ReadRequest() : ReadRequest(SHAKADB_INVALID_SERIES_ID,
                                         data_point_t::kMinTimestamp,
                                         data_point_t::kMinTimestamp) {
}

ReadRequest::ReadRequest(data_series_id_t series_id, timestamp_t begin, timestamp_t end) {
  this->series_id = series_id;
  this->begin = begin;
  this->end = end;
}

PacketType ReadRequest::GetType() {
  return kReadRequest;
}

data_series_id_t ReadRequest::GetSeriesId() {
  return this->series_id;
}

timestamp_t ReadRequest::GetBegin() {
  return this->begin;
}

timestamp_t ReadRequest::GetEnd() {
  return this->end;
}

bool ReadRequest::Deserialize(Buffer *payload) {
  if (payload->GetSize() != sizeof(read_request_t)) {
    return false;
  }

  read_request_t *request = reinterpret_cast<read_request_t *>(payload->GetBuffer());

  this->begin = request->begin;
  this->end = request->end;
  this->series_id = request->series_id;

  return true;
}

std::vector<Buffer *> ReadRequest::Serialize() {
  MemoryBuffer *buffer = new MemoryBuffer(sizeof(read_request_t));
  read_request_t *request = reinterpret_cast<read_request_t *>(buffer->GetBuffer());

  request->begin = this->begin;
  request->end = this->end;
  request->series_id = this->series_id;

  return std::vector<Buffer *> {buffer};
}

}  // namespace shakadb
