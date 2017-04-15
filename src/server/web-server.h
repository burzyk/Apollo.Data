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

#ifndef SRC_SERVER_WEB_SERVER_H_
#define SRC_SERVER_WEB_SERVER_H_

#include <list>
#include <map>
#include <src/storage/database.h>

#include "src/utils/threading.h"
#include "src/utils/threading.h"
#include "src/server/server.h"
#include "src/protocol.h"

namespace shakadb {

class WebServer : public Server {
 public:
  WebServer(int port, int backlog, int max_clients, int points_per_packet, sdb_database_t *db);
  ~WebServer();

  void Listen();

 private:
  static void WorkerRoutine(void *data);

  void HandleRead(sdb_socket_t client_socket, sdb_packet_t *packet);
  void HandleWrite(sdb_socket_t client_socket, sdb_packet_t *packet);

  int port;
  int backlog;
  sdb_thread_t **thread_pool;
  int thread_pool_size;
  sdb_database_t *db;
  int points_per_packet;
  int master_socket;
  volatile bool is_running;
};

}  // namespace shakadb

#endif  // SRC_SERVER_WEB_SERVER_H_
