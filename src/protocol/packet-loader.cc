//
// Created by Pawel Burzynski on 16/02/2017.
//

#include "packet-loader.h"
#include "ping-packet.h"
#include "write-request.h"

namespace shakadb {

DataPacket *PacketLoader::Load(Stream *stream) {
  data_packet_header_t header;

  if (stream->Peek((uint8_t *)&header, sizeof(header)) < sizeof(header)
      || stream->GetSize() < header.packet_length) {
    return nullptr;
  }

  switch (header.type) {
    case kPing: return new PingPacket(stream, header.packet_length);
    case kWrite: return new WriteRequest(stream, header.packet_length);
    default: return nullptr;
  }
}

}