//
// Created by Pawel Burzynski on 16/02/2017.
//

#include <src/utils/allocator.h>
#include <vector>
#include <src/utils/memory-buffer.h>
#include <src/utils/shallow-buffer.h>
#include "data-packet.h"
#include "ping-packet.h"
#include "write-request.h"
#include "read-request.h"
#include "read-response.h"
#include "write-response.h"

namespace shakadb {

DataPacket::DataPacket() {

}

DataPacket::~DataPacket() {
  for (auto fragment: this->fragments) {
    delete fragment;
  }
}

DataPacket *DataPacket::Load(Stream *stream) {
  data_packet_header_t header;

  if (stream->Read((byte_t *)&header, sizeof(data_packet_header_t)) < sizeof(data_packet_header_t)) {
    return nullptr;
  }

  if (header.packet_length > SHAKADB_PACKET_MAX_LEN) {
    return nullptr;
  }

  Buffer *raw_packet = new MemoryBuffer(header.packet_length);
  memcpy(raw_packet->GetBuffer(), &header, sizeof(data_point_t));
  byte_t *payload = raw_packet->GetBuffer() + sizeof(data_point_t);
  int payload_size = raw_packet->GetSize() - sizeof(data_point_t);

  if (stream->Read(payload, payload_size) != payload_size) {
    delete raw_packet;
    return nullptr;
  }

  DataPacket *result = nullptr;

  switch (header.type) {
    case kPing: result = new PingPacket();
      break;
    case kWriteRequest: result = new WriteRequest();
      break;
    case kWriteResponse: result = new WriteResponse();
      break;
    case kReadRequest: result = new ReadRequest();
      break;
    case kReadResponse: result = new ReadResponse();
      break;
    default: result = nullptr;
  }

  if (result == nullptr) {
    delete raw_packet;
    return nullptr;
  }

  result->fragments.push_back(raw_packet);
  ShallowBuffer buffer(
      raw_packet->GetBuffer() + sizeof(data_packet_header_t),
      raw_packet->GetSize() - sizeof(data_packet_header_t));

  if (!result->Deserialize(&buffer)) {
    delete result;
    result = nullptr;
  }

  return result;
}

std::vector<Buffer *> DataPacket::GetFragments() {
  if (this->fragments.size() > 0) {
    return this->fragments;
  }

  std::vector<Buffer *> payload = this->Serialize();
  int payload_size = 0;

  for (auto buffer: payload) {
    payload_size += buffer->GetSize();
  }

  MemoryBuffer *header_fragment = new MemoryBuffer(sizeof(data_packet_header_t));
  data_packet_header_t *header = (data_packet_header_t *)header_fragment->GetBuffer();
  header->type = this->GetType();
  header->packet_length = payload_size + header_fragment->GetSize();
  this->fragments.push_back(header_fragment);

  for (auto buffer: payload) {
    this->fragments.push_back(buffer);
  }

  return this->fragments;
}

}