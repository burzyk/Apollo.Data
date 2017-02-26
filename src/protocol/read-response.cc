//
// Created by Pawel Burzynski on 19/02/2017.
//

#include "read-response.h"

namespace shakadb {

ReadResponse::ReadResponse() : ReadResponse(nullptr, 0) {
}

ReadResponse::ReadResponse(DataPointsReader *reader, int front_discard) {
  this->reader = reader;
  this->points = nullptr;
  this->points_count = 0;

  if (reader != nullptr) {
    this->points = reader->GetDataPoints() + front_discard;
    this->points_count = max(0, reader->GetDataPointsCount() - front_discard);
  }
}

PacketType ReadResponse::GetType() {
  return kReadResponse;
}

int ReadResponse::GetPointsCount() {
  return this->points_count;
}

data_point_t *ReadResponse::GetPoints() {
  return this->points;
}

bool ReadResponse::Deserialize(Buffer *payload) {
  this->points = (data_point_t *)payload->GetBuffer();
  this->points_count = payload->GetSize() / sizeof(data_point_t);

  return true;
}

std::vector<Buffer *> ReadResponse::Serialize() {
  std::vector<Buffer *> result;

  if (this->points_count != 0) {
    ShallowBuffer *points_fragment = new ShallowBuffer(
        (byte_t *)this->points,
        this->points_count * sizeof(data_point_t));
    result.push_back(points_fragment);
  }

  return result;
}

}