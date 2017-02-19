//
// Created by Pawel Burzynski on 19/02/2017.
//

#ifndef SHAKADB_BASEHANDLER_H
#define SHAKADB_BASEHANDLER_H

#include <src/server/server-client.h>
#include <src/server/server.h>

namespace shakadb {

class BaseHandler : public Server::ServerListener, ServerClient::ServerClientListener {
 public:
  void OnClientConnected(Server *server, ServerClient *client);
  void OnSend(ServerClient *client, DataPacket *packet);
  void OnReceived(ServerClient *client, DataPacket *packet);
  void OnDisconnected(ServerClient *client);
};

}

#endif //SHAKADB_BASEHANDLER_H
