//
// Created by Pawel Burzynski on 25/02/2017.
//

#ifndef SHAKADB_SIMPLESERVER_H
#define SHAKADB_SIMPLESERVER_H

#include <src/protocol/data-packet.h>
#include <src/utils/thread.h>
#include <list>
#include <map>
#include <src/utils/socket-stream.h>
#include <src/utils/monitor.h>
#include <src/server/server.h>

namespace shakadb {

class WebServer : public Server {
 public:
  WebServer(int port, int backlog, int max_clients, Log *log);
  ~WebServer();

  void Listen();
  void Close();
  void AddServerListener(Server::ServerListener *listener);
  void SendPacket(int client_id, DataPacket *packet);
 private:
  struct client_info_t {
    SocketStream *socket;
    Monitor *lock;
  };

  void WorkerRoutine();
  int AllocateClient(SocketStream *socket);
  void CloseClient(int client_id);

  Log *log;
  int port;
  int backlog;
  int max_clients;
  std::list<Thread *> thread_pool;
  std::list<ServerListener *> listeners;
  int master_socket;
  volatile bool is_running;
  Monitor monitor;
  std::map<int, client_info_t *> clients;
  int next_client_id;

};

}

#endif //SHAKADB_SIMPLESERVER_H
