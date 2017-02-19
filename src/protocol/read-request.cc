//
// Created by Pawel Burzynski on 19/02/2017.
//

#include "read-request.h"

namespace shakadb {

ReadRequest::ReadRequest(uint8_t *raw_packet, int packet_size)
    : DataPacket(raw_packet, packet_size) {
}

ReadRequest::ReadRequest(std::string series_name, timestamp_t begin, timestamp_t end) {
  this->InitPacket(sizeof(int) + series_name.size() + 2 * sizeof(timestamp_t));
  int series_name_size = series_name.size();

  memcpy(this->GetPayload(), &series_name_size, sizeof(int));
  memcpy(this->GetPayload() + sizeof(int), series_name.c_str(), series_name.size());
  memcpy(this->GetPayload() + sizeof(int) + series_name.size(), &begin, sizeof(timestamp_t));
  memcpy(this->GetPayload() + sizeof(int) + series_name.size() + sizeof(timestamp_t), &end, sizeof(timestamp_t));
}

PacketType ReadRequest::GetType() {
  return kReadRequest;
}

std::string ReadRequest::GetSeriesName() {
  return std::string((char *)(this->GetPayload() + sizeof(int)), this->GetSeriesNameSize());
}

timestamp_t ReadRequest::GetBegin() {
  return *((timestamp_t *)(this->GetPayload() + sizeof(int) + this->GetSeriesNameSize()));
}

timestamp_t ReadRequest::GetEnd() {
  return *((timestamp_t *)(this->GetPayload() + sizeof(int) + this->GetSeriesNameSize() + sizeof(timestamp_t)));
}

int ReadRequest::GetSeriesNameSize() {
  return *((int *)this->GetPayload());
}

}