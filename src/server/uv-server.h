//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef APOLLO_STORAGE_UVSERVER_H
#define APOLLO_STORAGE_UVSERVER_H

#include <uv.h>
#include <src/utils/log.h>
#include <list>
#include <vector>
#include "client-handler.h"
#include "server.h"

namespace apollo {

class UvServer : public Server {
 public:
  UvServer(int port, int backlog, std::vector<ClientHandler *> handlers, Log *log);
  ~UvServer();

  void Listen();

  friend void on_new_connection(uv_stream_t *server, int status);
  friend void on_data_read(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf);
 private:
  uv_tcp_t *AddClient();
  void RemoveClient(uv_tcp_t *client);

  int port;
  int backlog;

  uv_loop_t event_loop;
  uv_tcp_t server;
  Log *log;
  std::vector<ClientHandler *> handlers;
  std::list<uv_tcp_t *> clients;
};

}

#endif //APOLLO_STORAGE_UVSERVER_H
