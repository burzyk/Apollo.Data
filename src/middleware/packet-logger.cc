//
// Created by Pawel Burzynski on 03/02/2017.
//

#include "packet-logger.h"

namespace shakadb {

PacketLogger::PacketLogger(Log *log) {
  this->log = log;
}

void PacketLogger::OnReceived(ServerClient *client, DataPacket *packet) {
  this->log->Debug(
      "Received packet: " + std::to_string(packet->GetType()) + " len: " + std::to_string(packet->GetPacketSize()));
}

}