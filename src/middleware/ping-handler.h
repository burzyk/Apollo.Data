//
// Created by Pawel Burzynski on 13/02/2017.
//

#ifndef SHAKADB_STORAGE_PINGHANDLER_H
#define SHAKADB_STORAGE_PINGHANDLER_H

#include <src/server/server.h>

namespace shakadb {

class PingHandler : public Server::ClientConnectedListener, ServerClient::ReceiveListener {
 public:
  void OnClientConnected(Server *server, ServerClient *client);
  void OnReceived(ServerClient *client, DataPacket *packet);
};

}

#endif //SHAKADB_STORAGE_PINGHANDLER_H
