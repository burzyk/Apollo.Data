//
// Created by Pawel Burzynski on 19/02/2017.
//

#include "read-response.h"

namespace shakadb {

ReadResponse::ReadResponse() : ReadResponse(nullptr) {
}

ReadResponse::ReadResponse(std::shared_ptr<DataPointsReader> reader) {
  this->reader = reader;

  if (reader != nullptr) {
    this->points = reader->GetDataPoints();
    this->points_count = reader->GetDataPointsCount();
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

  if (this->points != nullptr) {
    ShallowBuffer *points_fragment = new ShallowBuffer(
        (byte_t *)this->points,
        this->points_count * sizeof(data_point_t));
    result.push_back(points_fragment);
  }

  return result;
}

}