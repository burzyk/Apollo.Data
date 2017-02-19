//
// Created by Pawel Burzynski on 16/02/2017.
//

#include <src/utils/allocator.h>
#include "packet-loader.h"
#include "ping-packet.h"
#include "write-request.h"

namespace shakadb {

DataPacket *PacketLoader::Load(Stream *stream) {
  data_packet_header_t header;

  if (stream->Peek((uint8_t *)&header, sizeof(data_packet_header_t)) < sizeof(data_packet_header_t)
      || !stream->HasData(header.packet_length)) {
    return nullptr;
  }

  uint8_t *raw_packet = Allocator::New<uint8_t>(header.packet_length);

  if (stream->Read(raw_packet, header.packet_length) != header.packet_length) {
    throw FatalException("Not enough data in the buffer");
  }

  return Load(raw_packet, header.packet_length);
}

DataPacket *PacketLoader::Load(uint8_t *raw_packet, int packet_size) {
  data_packet_header_t *header = (data_packet_header_t *)raw_packet;

  switch (header->type) {
    case kPing: return new PingPacket(raw_packet, packet_size);
    case kWriteRequest: return new WriteRequest(raw_packet, packet_size);
    default: return nullptr;
  }
}

}