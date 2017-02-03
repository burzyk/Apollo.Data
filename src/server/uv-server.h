//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef APOLLO_STORAGE_UVSERVER_H
#define APOLLO_STORAGE_UVSERVER_H

#include <uv.h>
#include <src/utils/log.h>
#include <list>
#include <vector>
#include <map>
#include "client-handler.h"
#include "server.h"

namespace apollo {

#define A_SERVER_BUFFER_SIZE  65536

class UvServer : public Server {
 public:
  UvServer(int port, int backlog, std::vector<ClientHandler *> handlers, Log *log);
  ~UvServer();

  void Listen();

  friend void on_new_connection(uv_stream_t *server, int status);
  friend void on_data_read(uv_stream_t *server, ssize_t nread, const uv_buf_t *buf);
 private:
  struct client_info_t {
    int id;
    uint8_t *buffer;
    ssize_t buffer_size;
    ssize_t buffer_position;
  };

  void RegisterClient(uv_tcp_t *client);
  void RemoveClient(uv_tcp_t *client);
  void HandlePacket(data_packet_t *packet);

  int port;
  int backlog;
  int current_client_id;

  uv_loop_t event_loop;
  uv_tcp_t server;
  Log *log;
  std::vector<ClientHandler *> handlers;
  std::map<uv_tcp_t *, client_info_t *> clients;
};

}

#endif //APOLLO_STORAGE_UVSERVER_H
