//
// Created by Pawel Burzynski on 03/02/2017.
//

#include "packet-logger.h"

namespace apollo {

PacketLogger::PacketLogger(Log *log) {
  this->log = log;
}

void PacketLogger::ProcessPacket(data_packet_t *packet, ClientResponder *responder) {
  this->log->Info("Received packet: " + std::to_string(packet->type) + " len: " + std::to_string(packet->total_length));
}

}