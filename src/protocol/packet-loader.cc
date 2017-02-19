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

  if (stream->Peek((uint8_t *)&header, sizeof(header)) < sizeof(header)
      || !stream->HasData(header.packet_length)) {
    return nullptr;
  }

  std::shared_ptr<uint8_t> raw_packet = std::shared_ptr<uint8_t>(Allocator::New<uint8_t>());

  if (stream->Read(raw_packet.get(), header.packet_length) != header.packet_length) {
    throw FatalException("Not enough data in the buffer");
  }

  return Load(raw_packet, header.packet_length);
}

DataPacket *PacketLoader::Load(std::shared_ptr<uint8_t> raw_packet, int packet_size) {
  data_packet_header_t *header = (data_packet_header_t *)raw_packet.get();

  switch (header->type) {
    case kPing: return new PingPacket(raw_packet, packet_size);
    case kWrite: return new WriteRequest(raw_packet, packet_size);
    default: return nullptr;
  }
}

}