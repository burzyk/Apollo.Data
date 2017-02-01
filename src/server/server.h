//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef APOLLO_STORAGE_SERVER_H
#define APOLLO_STORAGE_SERVER_H

#include <vector>
#include <src/utils/log.h>
#include <list>
#include "client-handler.h"

namespace apollo {

struct server_info_t {
  uv_loop_t event_loop;
  uv_tcp_t server;
  Log *log;
  std::vector<ClientHandler *> handlers;
  std::list<uv_tcp_t *> clients;
};

class Server {
 public :
  Server(int port, int backlog, std::vector<ClientHandler *> handlers, Log *log);
  ~Server();

  void Listen();
 private:
  int port;
  int backlog;
  server_info_t info;
};

}

#endif //APOLLO_STORAGE_SERVER_H
