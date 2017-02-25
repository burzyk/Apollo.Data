//
// Created by Pawel Burzynski on 16/02/2017.
//

#include <cstring>
#include <src/utils/memory-buffer.h>
#include "ping-packet.h"

namespace shakadb {

PingPacket::PingPacket(char *ping_data, int ping_data_size)
    : DataPacket(kPing, ping_data_size) {
  MemoryBuffer *payload = new MemoryBuffer(ping_data_size);
  memcpy(payload->GetBuffer(), ping_data, payload->GetSize());
  this->AddFragment(payload);
}

PingPacket::PingPacket(Buffer *packet)
    : DataPacket(packet) {
}

PacketType PingPacket::GetType() {
  return kPing;
}

char *PingPacket::GetPingData() {
}

int PingPacket::GetPingDataSize() {
}

}
