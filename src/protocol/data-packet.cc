//
// Created by Pawel Burzynski on 16/02/2017.
//

#include <src/utils/allocator.h>
#include "data-packet.h"

namespace shakadb {

DataPacket::DataPacket() {
//  this->raw_packet = nullptr;
  this->packet_size = 0;
}

DataPacket::DataPacket(std::shared_ptr<uint8_t> raw_packet, int packet_size) {
  this->raw_packet = raw_packet;
  this->packet_size = packet_size;
}

uint8_t *DataPacket::GetPacket() {
  return this->raw_packet.get();
}

int DataPacket::GetPacketSize() {
  return this->packet_size;
}

void DataPacket::InitPacket(int payload_size) {
//  this->packet_size = payload_size + sizeof(data_packet_header_t);
//  this->raw_packet = Allocator::New<uint8_t>(this->packet_size);
//
//  data_packet_header_t *header = (data_packet_header_t *)this->raw_packet;
//  header->packet_length = this->packet_size;
//  header->type = this->GetType();
}

uint8_t *DataPacket::GetPayload() {
  return this->raw_packet.get() + sizeof(data_packet_header_t);
}

int DataPacket::GetPayloadSize() {
  return this->packet_size - sizeof(data_packet_header_t);
}

}