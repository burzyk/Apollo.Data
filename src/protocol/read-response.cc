//
// Created by Pawel Burzynski on 19/02/2017.
//

#include "read-response.h"

namespace shakadb {

ReadResponse::ReadResponse(uint8_t *raw_packet, int packet_size)
    : DataPacket(raw_packet, packet_size) {
}

ReadResponse::ReadResponse(data_point_t *points, int points_count, int total_points_count) {
  this->InitPacket(sizeof(int) + sizeof(data_point_t) * points_count);

  memcpy(this->GetPayload(), &total_points_count, sizeof(int));

  if (points_count != 0) {
    memcpy(this->GetPayload() + sizeof(int), points, points_count * sizeof(data_point_t));
  }
}

PacketType ReadResponse::GetType() {
  return kReadResponse;
}

int ReadResponse::GetPointsCount() {
  return (this->GetPayloadSize() - sizeof(int)) / sizeof(data_point_t);
}

data_point_t *ReadResponse::GetPoints() {
  return (data_point_t *)(this->GetPayload() + sizeof(int));
}

int ReadResponse::GetTotalPointsCount() {
  return *((int *)this->GetPayload());
}

}