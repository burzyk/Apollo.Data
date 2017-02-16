//
// Created by Pawel Burzynski on 03/02/2017.
//

#ifndef SHAKADB_STORAGE_PACKETLOGGER_H
#define SHAKADB_STORAGE_PACKETLOGGER_H

#include <src/log.h>
#include <src/server/server.h>

namespace shakadb {

class PacketLogger : public Server::ClientConnectedListener, ServerClient::ReceiveListener {
 public:
  PacketLogger(Log *log);

  void OnClientConnected(Server *server, ServerClient *client);
  void OnReceived(ServerClient *client, data_packet_t *packet);
 private:
  Log *log;
};

}

#endif //SHAKADB_STORAGE_PACKETLOGGER_H
