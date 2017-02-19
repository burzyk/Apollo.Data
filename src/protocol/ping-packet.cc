//
// Created by Pawel Burzynski on 16/02/2017.
//

#include <cstring>
#include "ping-packet.h"

namespace shakadb {

PingPacket::PingPacket(uint8_t *raw_packet, int packet_size)
    : DataPacket(raw_packet, packet_size) {
}

PingPacket::PingPacket(char *ping_data, int ping_data_size)
    : DataPacket() {
  this->InitPacket(ping_data_size);
  memcpy(this->GetPayload(), ping_data, ping_data_size);
}

PacketType PingPacket::GetType() {
  return kPing;
}

char *PingPacket::GetPingData() {
  return (char *)this->GetPayload();
}

int PingPacket::GetPingDataSize() {
  return this->GetPayloadSize();
}

}
