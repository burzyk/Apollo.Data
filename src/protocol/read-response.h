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

#ifndef SRC_PROTOCOL_READ_RESPONSE_H_
#define SRC_PROTOCOL_READ_RESPONSE_H_

#include <vector>

#include "src/data-point.h"
#include "src/utils/common.h"
#include "src/storage/data-points-reader.h"
#include "src/utils/shallow-buffer.h"
#include "src/protocol/data-packet.h"

namespace shakadb {

class ReadResponse : public DataPacket {
 public:
  ReadResponse();
  ReadResponse(DataPointsReader *reader, int front_discard);

  PacketType GetType();
  int GetPointsCount();
  data_point_t *GetPoints();

 protected:
  bool Deserialize(Buffer *payload);
  std::vector<Buffer *> Serialize();

 private:
  data_point_t *points;
  int points_count;
  DataPointsReader *reader;
};

}  // namespace shakadb

#endif  // SRC_PROTOCOL_READ_RESPONSE_H_
