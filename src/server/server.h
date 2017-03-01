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

#ifndef SRC_SERVER_SERVER_H_
#define SRC_SERVER_SERVER_H_

#include <vector>
#include <list>

#include "src/log.h"
#include "src/protocol/data-packet.h"

namespace shakadb {

class Server {
 public:
  class ServerListener {
   public:
    virtual ~ServerListener() {}
    virtual void OnClientConnected(int client_id) = 0;
    virtual void OnClientDisconnected(int client_id) = 0;
    virtual void OnPacketReceived(int client_id, DataPacket *packet) = 0;
  };

  virtual ~Server() {}
  virtual void Listen() = 0;
  virtual void Close() = 0;
  virtual void AddServerListener(ServerListener *listener) = 0;
  virtual bool SendPacket(int client_id, DataPacket *packet) = 0;
};

}  // namespace shakadb

#endif  // SRC_SERVER_SERVER_H_
