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

#include "src/protocol/ping-packet.h"

#include <cstring>

#include "src/utils/memory-buffer.h"

namespace shakadb {

PingPacket::PingPacket() : PingPacket(nullptr, 0) {
}

PingPacket::PingPacket(char *ping_data, int ping_data_size) {
  this->ping_data = ping_data;
  this->ping_data_size = ping_data_size;
}

PacketType PingPacket::GetType() {
  return kPing;
}

char *PingPacket::GetPingData() {
  return this->ping_data;
}

int PingPacket::GetPingDataSize() {
  return this->ping_data_size;
}

bool PingPacket::Deserialize(Buffer *payload) {
  this->ping_data_size = payload->GetSize();
  this->ping_data = reinterpret_cast<char *>(payload->GetBuffer());

  return true;
}

std::vector<Buffer *> PingPacket::Serialize() {
  MemoryBuffer *buffer = new MemoryBuffer(this->ping_data_size);
  memcpy(buffer->GetBuffer(), this->ping_data, this->ping_data_size);

  return std::vector<Buffer *> {buffer};
}

}  // namespace shakadb
