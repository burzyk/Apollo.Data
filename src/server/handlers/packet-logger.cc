//
// Created by Pawel Burzynski on 03/02/2017.
//

#include "packet-logger.h"

namespace apollo {

PacketLogger::PacketLogger(Log *log) {
  this->log = log;
}

void PacketLogger::OnClientConnected(Server *server, ServerClient *client) {
  client->AddReceivedListener(this);
}

void PacketLogger::OnReceived(ServerClient *client, data_packet_t *packet) {
  this->log->Debug(
      "Received packet: " + std::to_string(packet->type) + " len: " + std::to_string(packet->total_length));
}

}