//
// Created by Pawel Burzynski on 19/02/2017.
//

#include "read-response.h"

namespace shakadb {

ReadResponse::ReadResponse(Buffer *packet) : DataPacket(packet) {
}

ReadResponse::ReadResponse(data_point_t *points, int points_count, int total_points_count)
    : DataPacket(kReadResponse, 0) {
}

PacketType ReadResponse::GetType() {
  return kReadResponse;
}

int ReadResponse::GetPointsCount() {
}

data_point_t *ReadResponse::GetPoints() {
}

int ReadResponse::GetTotalPointsCount() {
}

}