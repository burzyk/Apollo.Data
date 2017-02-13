//
// Created by Pawel Burzynski on 13/02/2017.
//

#ifndef APOLLO_STORAGE_PINGHANDLER_H
#define APOLLO_STORAGE_PINGHANDLER_H

#include <src/server/server.h>

namespace apollo {

class PingHandler : public Server::ClientConnectedListener, ServerClient::ReceiveListener {
 public:
  void OnClientConnected(Server *server, ServerClient *client);
  void OnReceived(ServerClient *client, data_packet_t *packet);
};

}

#endif //APOLLO_STORAGE_PINGHANDLER_H
