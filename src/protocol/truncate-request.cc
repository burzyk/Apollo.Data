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
// Created by Pawel Burzynski on 08/03/2017.
//

#include "src/protocol/truncate-request.h"

#include "src/utils/memory-buffer.h"

namespace shakadb {

TruncateRequest::TruncateRequest() : TruncateRequest("") {
}

TruncateRequest::TruncateRequest(std::string series_name) {
  this->series_name = series_name;
}

PacketType TruncateRequest::GetType() {
  return kTruncateRequest;
}

std::string TruncateRequest::GetSeriesName() {
  return this->series_name;
}

bool TruncateRequest::Deserialize(Buffer *payload) {
  if (payload->GetSize() != sizeof(truncate_request_t)) {
    return false;
  }

  truncate_request_t *request = reinterpret_cast<truncate_request_t *>(payload->GetBuffer());

  this->series_name = std::string(request->series_name);

  return true;
}

std::vector<Buffer *> TruncateRequest::Serialize() {
  MemoryBuffer *buffer = new MemoryBuffer(sizeof(truncate_request_t));
  truncate_request_t *request = reinterpret_cast<truncate_request_t *>(buffer->GetBuffer());

  memcpy(request->series_name, this->series_name.c_str(), this->series_name.size());

  return std::vector<Buffer *> {buffer};
}

}  // namespace shakadb
