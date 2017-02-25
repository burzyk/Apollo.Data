//
// Created by Pawel Burzynski on 16/02/2017.
//

#include <src/utils/allocator.h>
#include <vector>
#include <src/utils/memory-buffer.h>
#include "data-packet.h"
#include "ping-packet.h"
#include "write-request.h"
#include "read-request.h"
#include "read-response.h"

namespace shakadb {

DataPacket::DataPacket(PacketType type, int payload_size) {
  data_packet_header_t header{.type = type, .packet_length = payload_size + (int)sizeof(data_packet_header_t)};
  MemoryBuffer *first_fragment = new MemoryBuffer(sizeof(data_packet_header_t));

  memcpy(first_fragment->GetBuffer(), &header, first_fragment->GetSize());

  this->AddFragment(first_fragment);
}

DataPacket::DataPacket(Buffer *packet) {
  this->fragments.push_back(packet);
}

DataPacket::~DataPacket() {
  for (auto fragment: this->fragments) {
    delete fragment;
  }
}

DataPacket *DataPacket::Load(Stream *stream) {
  data_packet_header_t header;

  if (stream->Peek((byte_t *)&header, sizeof(data_packet_header_t)) < sizeof(data_packet_header_t)
      || !stream->HasData(header.packet_length)) {
    return nullptr;
  }

  Buffer *raw_packet = new MemoryBuffer(header.packet_length);

  if (stream->Read(raw_packet->GetBuffer(), header.packet_length) != header.packet_length) {
    throw FatalException("Not enough data in the buffer");
  }

  switch (header.type) {
    case kPing: return new PingPacket(raw_packet);
    case kWriteRequest: return new WriteRequest(raw_packet);
    case kReadRequest: return new ReadRequest(raw_packet);
    case kReadResponse: return new ReadResponse(raw_packet);
    default: return nullptr;
  }
}

std::vector<Buffer *> DataPacket::GetFragments() {
  return this->fragments;
}

void DataPacket::AddFragment(Buffer *fragment) {
  this->fragments.push_back(fragment);
}

}