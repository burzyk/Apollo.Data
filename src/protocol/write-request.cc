//
// Created by Pawel Burzynski on 16/02/2017.
//

#include "write-request.h"

namespace shakadb {

WriteRequest::WriteRequest(RingBuffer *buffer, int packet_size)
    : DataPacket(buffer, packet_size) {
}

WriteRequest::WriteRequest(std::string series_name, data_point_t *points, int points_count)
    : DataPacket() {
  this->InitPacket(sizeof(int) + series_name.size() + points_count * sizeof(data_point_t));
  int series_name_size = series_name.size();

  memcpy(this->GetPayload(), &series_name_size, sizeof(int));
  memcpy(this->GetPayload() + sizeof(int), series_name.c_str(), series_name.size());
  memcpy(this->GetPayload() + sizeof(int) + series_name.size(), points, points_count * sizeof(data_point_t));
}

PacketType WriteRequest::GetType() {
  return kWrite;
}

int WriteRequest::GetPointsCount() {
  return (this->GetPayloadSize() - sizeof(int) - this->GetSeriesNameSize()) / sizeof(data_point_t);
}

data_point_t *WriteRequest::GetPoints() {
  return (data_point_t *)(this->GetPayload() + sizeof(int) + this->GetSeriesNameSize());
}

std::string WriteRequest::GetSeriesName() {
  return std::string((char *)(this->GetPayload() + sizeof(int)), this->GetSeriesNameSize());
}

int WriteRequest::GetSeriesNameSize() {
  return *((int *)this->GetPayload());
}

}