//
// Created by Pawel Burzynski on 19/02/2017.
//

#include "read-response.h"

namespace shakadb {

ReadResponse::ReadResponse(uint8_t *raw_packet, int packet_size)
    : DataPacket(raw_packet, packet_size) {
}

ReadResponse::ReadResponse(data_point_t *points, int points_count) {

}

PacketType ReadResponse::GetType() {
  return kReadResponse;
}

int ReadResponse::GetPointsCount() {
  return this->GetPayloadSize() / sizeof(data_point_t);
}

data_point_t *ReadResponse::GetPoints() {
  return (data_point_t *)this->GetPayload();
}

}