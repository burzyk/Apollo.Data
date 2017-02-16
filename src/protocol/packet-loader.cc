//
// Created by Pawel Burzynski on 16/02/2017.
//

#include "packet-loader.h"
#include "ping-packet.h"
#include "write-request.h"

namespace shakadb {

DataPacket *PacketLoader::Load(RingBuffer *buffer) {
  data_packet_header_t header;

  if (buffer->Peek((uint8_t *)&header, sizeof(header)) < sizeof(header)
      || buffer->GetSize() < header.packet_length) {
    return nullptr;
  }

  switch (header.type) {
    case kPing: return new PingPacket(buffer, header.packet_length);
    case kWrite: return new WriteRequest(buffer, header.packet_length);
    default: return nullptr;
  }
}

}