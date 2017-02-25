//
// Created by Pawel Burzynski on 03/02/2017.
//

#include "packet-logger.h"

namespace shakadb {

PacketLogger::PacketLogger(Server *server, Log *log)
    : BaseHandler(server) {
  this->log = log;
}

void PacketLogger::OnPacketReceived(int client_id, DataPacket *packet) {
  this->log->Debug("Received packet: " + std::to_string(packet->GetType()));
}

}