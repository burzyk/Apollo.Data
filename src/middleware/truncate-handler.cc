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

#include "src/middleware/truncate-handler.h"

#include "src/protocol/truncate-request.h"
#include "src/protocol/simple-response.h"
#include "src/fatal-exception.h"

namespace shakadb {

TruncateHandler::TruncateHandler(Database *db, Server *server)
    : BaseHandler(server) {
  this->db = db;
}

void TruncateHandler::OnPacketReceived(int client_id, DataPacket *packet) {
  if (packet->GetType() != kTruncateRequest) {
    return;
  }

  TruncateRequest *request = static_cast<TruncateRequest *>(packet);
  this->db->Truncate(request->GetSeriesName());

  SimpleResponse response(kOk);
  this->GetServer()->SendPacket(client_id, &response);
}

}  // namespace shakadb