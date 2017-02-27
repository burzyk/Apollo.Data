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
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef SHAKADB_STORAGE_DATA_PACKET_H
#define SHAKADB_STORAGE_DATA_PACKET_H

#include <cstdint>
#include <src/utils/ring-buffer.h>
#include <memory>
#include <vector>
#include <src/utils/buffer.h>

namespace shakadb {

enum PacketType {
  kPing = 1,
  kWriteRequest = 2,
  kWriteResponse = 3,
  kReadRequest = 4,
  kReadResponse = 5
};

struct data_packet_header_t {
  PacketType type;
  uint32_t packet_length;
};

class DataPacket {
 public:
  DataPacket();
  virtual ~DataPacket();
  static DataPacket *Load(Stream *stream);

  virtual PacketType GetType() = 0;
  std::vector<Buffer *> GetFragments();
 protected:
  virtual bool Deserialize(Buffer *payload) = 0;
  virtual std::vector<Buffer *> Serialize() = 0;
 private:
  std::vector<Buffer *> fragments;
};

}

#endif //SHAKADB_STORAGE_DATA_PACKET_H
