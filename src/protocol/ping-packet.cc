//
// Created by Pawel Burzynski on 16/02/2017.
//

#include <cstring>
#include <src/utils/memory-buffer.h>
#include "ping-packet.h"

namespace shakadb {

PingPacket::PingPacket() : PingPacket(nullptr, 0) {
}

PingPacket::PingPacket(char *ping_data, int ping_data_size) {
  this->ping_data = ping_data;
  this->ping_data_size = ping_data_size;
}

PacketType PingPacket::GetType() {
  return kPing;
}

char *PingPacket::GetPingData() {
  return this->ping_data;
}

int PingPacket::GetPingDataSize() {
  return this->ping_data_size;
}

bool PingPacket::Deserialize(Buffer *payload) {
  this->ping_data_size = payload->GetSize();
  this->ping_data = (char *)payload->GetBuffer();

  return true;
}

std::vector<Buffer *> PingPacket::Serialize() {
  MemoryBuffer *buffer = new MemoryBuffer(this->ping_data_size);
  memcpy(buffer->GetBuffer(), this->ping_data, this->ping_data_size);

  return std::vector<Buffer *> {buffer};
}

}
