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

#include "src/protocol/data-packet.h"

#include <memory.h>
#include <vector>

#include "src/utils/allocator.h"
#include "src/utils/memory-buffer.h"
#include "src/utils/shallow-buffer.h"
#include "src/protocol/ping-packet.h"
#include "src/protocol/write-request.h"
#include "src/protocol/read-request.h"
#include "src/protocol/read-response.h"
#include "src/protocol/simple-response.h"
#include "src/protocol/truncate-request.h"

namespace shakadb {

DataPacket::DataPacket() {
}

DataPacket::~DataPacket() {
  for (auto fragment : this->fragments) {
    delete fragment;
  }
}

DataPacket *DataPacket::Load(Stream *stream) {
  data_packet_header_t header;

  if (stream->Read(reinterpret_cast<byte_t *>(&header), sizeof(data_packet_header_t)) < sizeof(data_packet_header_t)) {
    return nullptr;
  }

  if (header.packet_length > SHAKADB_PACKET_MAX_LEN) {
    return nullptr;
  }

  Buffer *raw_packet = new MemoryBuffer(header.packet_length);
  memcpy(raw_packet->GetBuffer(), &header, sizeof(data_packet_header_t));
  byte_t *payload = raw_packet->GetBuffer() + sizeof(data_packet_header_t);
  int payload_size = raw_packet->GetSize() - sizeof(data_packet_header_t);

  if (stream->Read(payload, payload_size) != payload_size) {
    delete raw_packet;
    return nullptr;
  }

  DataPacket *result = nullptr;

  switch (header.type) {
    case kPing: result = new PingPacket();
      break;
    case kWriteRequest: result = new WriteRequest();
      break;
    case kSimpleResponse: result = new SimpleResponse();
      break;
    case kReadRequest: result = new ReadRequest();
      break;
    case kReadResponse: result = new ReadResponse();
      break;
    case kTruncateRequest: result = new TruncateRequest();
      break;
    default: result = nullptr;
  }

  if (result == nullptr) {
    delete raw_packet;
    return nullptr;
  }

  result->fragments.push_back(raw_packet);
  ShallowBuffer buffer(
      raw_packet->GetBuffer() + sizeof(data_packet_header_t),
      raw_packet->GetSize() - sizeof(data_packet_header_t));

  if (!result->Deserialize(&buffer)) {
    delete result;
    result = nullptr;
  }

  return result;
}

std::vector<Buffer *> DataPacket::GetFragments() {
  if (this->fragments.size() > 0) {
    return this->fragments;
  }

  std::vector<Buffer *> payload = this->Serialize();
  int payload_size = 0;

  for (auto buffer : payload) {
    payload_size += buffer->GetSize();
  }

  MemoryBuffer *header_fragment = new MemoryBuffer(sizeof(data_packet_header_t));
  data_packet_header_t *header = reinterpret_cast<data_packet_header_t *>(header_fragment->GetBuffer());
  header->type = this->GetType();
  header->packet_length = payload_size + header_fragment->GetSize();
  this->fragments.push_back(header_fragment);

  for (auto buffer : payload) {
    this->fragments.push_back(buffer);
  }

  return this->fragments;
}

}  // namespace shakadb
