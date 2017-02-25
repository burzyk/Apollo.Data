//
// Created by Pawel Burzynski on 16/02/2017.
//

#include "write-request.h"

namespace shakadb {

WriteRequest::WriteRequest(std::string series_name, data_point_t *points, int points_count)
    : DataPacket(kWriteRequest, 0) {
}

WriteRequest::WriteRequest(Buffer *packet)
    : DataPacket(packet) {
}

PacketType WriteRequest::GetType() {
  return kWriteRequest;
}

int WriteRequest::GetPointsCount() {
}

data_point_t *WriteRequest::GetPoints() {
}

std::string WriteRequest::GetSeriesName() {
}

int WriteRequest::GetSeriesNameSize() {
}

}