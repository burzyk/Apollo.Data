//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef SHAKADB_STORAGE_UVSERVER_H
#define SHAKADB_STORAGE_UVSERVER_H

#include <uv.h>
#include <src/utils/log.h>
#include <list>
#include <vector>
#include <map>
#include "server.h"
#include "uv-server-client.h"

namespace shakadb {

class UvServer : public Server {
 public:
  UvServer(int port, int backlog, Log *log);

  void Listen();
  void Close();
  void AddClientConnectedListener(ClientConnectedListener *listener);
 private:
  static void OnNewConnection(uv_stream_t *server, int status);
  static void OnServerClose(uv_async_t *handle);

  void CleanDisconnectedClients();

  int port;
  int backlog;
  bool is_running;

  std::list<UvServerClient *> clients;
  std::list<Server::ClientConnectedListener *> client_connected_listeners;

  uv_loop_t event_loop;
  uv_tcp_t server;
  Log *log;
};

}

#endif //SHAKADB_STORAGE_UVSERVER_H
