//
// Created by Pawel Burzynski on 13/02/2017.
//

#include <src/protocol/ping-packet.h>
#include "ping-handler.h"

namespace shakadb {

void PingHandler::OnReceived(ServerClient *client, DataPacket *packet) {
  if (packet->GetType() != PacketType::kPing) {
    return;
  }

  PingPacket *request = (PingPacket *)packet;
  std::shared_ptr<DataPacket> response = std::shared_ptr<DataPacket>(
      new PingPacket(request->GetPingData(), request->GetPingDataSize()));

  client->SendPacket(response);
}

}