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

#include "src/middleware/read-handler.h"

#include <memory>

#include "src/protocol/read-request.h"
#include "src/protocol/read-response.h"
#include "src/utils/allocator.h"

namespace shakadb {

ReadHandler::ReadHandler(Database *db, Server *server, int points_per_packet)
    : BaseHandler(server) {
  this->db = db;
  this->points_per_packet = points_per_packet;
}

void ReadHandler::OnPacketReceived(int client_id, DataPacket *packet) {
  if (packet->GetType() != kReadRequest) {
    return;
  }

  ReadRequest *request = static_cast<ReadRequest *>(packet);
  timestamp_t begin = request->GetBegin();

  while (true) {
    auto reader = std::unique_ptr<DataPointsReader>(
        this->db->Read(request->GetSeriesName(), begin, request->GetEnd(), this->points_per_packet));
    ReadResponse response(reader.get(), begin == request->GetBegin() ? 0 : 1);

    if (!this->GetServer()->SendPacket(client_id, &response)) {
      return;
    }

    if (response.GetPointsCount() == 0) {
      break;
    }

    begin = reader->GetDataPoints()[reader->GetDataPointsCount() - 1].time;
  }
}

}  // namespace shakadb
