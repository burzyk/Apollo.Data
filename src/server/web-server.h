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

#include "src/utils/threading.h"
#include "src/utils/monitor.h"
#include "src/server/server.h"
#include "src/protocol.h"

namespace shakadb {

class WebServer : public Server {
 public:
  WebServer(int port, int backlog, int max_clients, Log *log);
  ~WebServer();

  void Listen();
  void Close();
  void AddServerListener(Server::ServerListener *listener);
  bool SendPacket(int client_id, sdb_packet_t *packet);

 private:
  struct client_info_t {
    sdb_socket_t socket;
    Monitor *lock;
  };

  static void WorkerRoutine(void *data);
  int AllocateClient(sdb_socket_t socket);
  void CloseClient(int client_id);

  Log *log;
  int port;
  int backlog;
  int max_clients;
  std::list<sdb_thread_t *> thread_pool;
  std::list<ServerListener *> listeners;
  int master_socket;
  volatile bool is_running;
  Monitor monitor;
  std::map<int, client_info_t *> clients;
  int next_client_id;
};

}  // namespace shakadb

#endif  // SRC_SERVER_WEB_SERVER_H_
