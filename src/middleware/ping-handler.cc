//
// Created by Pawel Burzynski on 13/02/2017.
//

#include "ping-handler.h"

namespace shakadb {

void PingHandler::OnClientConnected(Server *server, ServerClient *client) {
  client->AddReceivedListener(this);
}

void PingHandler::OnReceived(ServerClient *client, data_packet_t *packet) {
  if (packet->type != PacketType::kPing) {
    return;
  }

  client->SendPacket(PacketType::kPing, packet->data, packet->payload_length());
}

}