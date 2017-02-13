//
// Created by Pawel Burzynski on 03/02/2017.
//

#ifndef APOLLO_STORAGE_PACKETLOGGER_H
#define APOLLO_STORAGE_PACKETLOGGER_H

#include <src/utils/log.h>
#include <src/server/server.h>

namespace apollo {

class PacketLogger : public Server::ClientConnectedListener, ServerClient::ReceiveListener {
 public:
  PacketLogger(Log *log);

  void OnClientConnected(Server *server, ServerClient *client);
  void OnReceived(ServerClient *client, data_packet_t *packet);
 private:
  Log *log;
};

}

#endif //APOLLO_STORAGE_PACKETLOGGER_H
