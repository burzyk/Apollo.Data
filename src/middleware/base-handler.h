//
// Created by Pawel Burzynski on 19/02/2017.
//

#ifndef SHAKADB_BASEHANDLER_H
#define SHAKADB_BASEHANDLER_H

#include <src/server/server.h>

namespace shakadb {

class BaseHandler : public Server::ServerListener {
 public:
  BaseHandler(Server *server);

  void OnClientConnected(int client_id);
  void OnClientDisconnected(int client_id);
  void OnPacketReceived(int client_id, DataPacket *packet);
  Server *GetServer();
 private:
  Server *server;
};

}

#endif //SHAKADB_BASEHANDLER_H
