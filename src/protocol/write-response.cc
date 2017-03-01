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
// Created by Pawel Burzynski on 25/02/2017.
//

#include "src/protocol/write-response.h"

#include "src/utils/memory-buffer.h"

namespace shakadb {

WriteResponse::WriteResponse() {
}

WriteResponse::WriteResponse(WriteStatus status) {
  this->status = status;
}

PacketType WriteResponse::GetType() {
  return kWriteResponse;
}

WriteStatus WriteResponse::GetStatus() {
  return this->status;
}

bool WriteResponse::Deserialize(Buffer *payload) {
  if (payload->GetSize() != sizeof(write_response_t)) {
    return false;
  }

  write_response_t *response = reinterpret_cast<write_response_t *>(payload->GetBuffer());

  this->status = response->status;

  return true;
}

std::vector<Buffer *> WriteResponse::Serialize() {
  MemoryBuffer *buffer = new MemoryBuffer(sizeof(write_response_t));
  write_response_t *response = reinterpret_cast<write_response_t *>(buffer->GetBuffer());

  response->status = this->status;

  return std::vector<Buffer *> {buffer};
}

}  // namespace shakadb
