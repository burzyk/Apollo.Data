//
// Created by Pawel Burzynski on 13/02/2017.
//

#include <src/protocol/ping-packet.h>
#include "ping-handler.h"

namespace shakadb {

PingHandler::PingHandler(Server * server)
    : BaseHandler(server) {
}

void PingHandler::OnPacketReceived(int client_id, DataPacket *packet) {
  if (packet->GetType() != PacketType::kPing) {
    return;
  }

  PingPacket *request = (PingPacket *)packet;
  PingPacket response(request->GetPingData(), request->GetPingDataSize());

  this->GetServer()->SendPacket(client_id, &response);
}

}