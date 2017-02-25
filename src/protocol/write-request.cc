//
// Created by Pawel Burzynski on 16/02/2017.
//

#include <src/utils/memory-buffer.h>
#include <src/utils/shallow-buffer.h>
#include "write-request.h"

namespace shakadb {

WriteRequest::WriteRequest() : WriteRequest("", nullptr, 0) {
}

WriteRequest::WriteRequest(std::string series_name, data_point_t *points, int points_count) {
  this->series_name = series_name;
  this->points = points;
  this->points_count = points_count;
}

PacketType WriteRequest::GetType() {
  return kWriteRequest;
}

int WriteRequest::GetPointsCount() {
  return this->points_count;
}

data_point_t *WriteRequest::GetPoints() {
  return this->points;
}

std::string WriteRequest::GetSeriesName() {
  return this->series_name;
}

bool WriteRequest::Deserialize(Buffer *payload) {
  if (payload->GetSize() < sizeof(write_request_t)) {
    return false;
  }

  write_request_t request;
  memcpy(&request, payload->GetBuffer(), sizeof(write_request_t));

  this->points_count = request.points_count;
  this->series_name = std::string(request.series_name);

  if (payload->GetSize() != sizeof(write_request_t) + this->points_count * sizeof(data_point_t)) {
    return false;
  }

  this->points = (data_point_t *)payload->GetBuffer() + sizeof(write_request_t);

  return true;
}

std::vector<Buffer *> WriteRequest::Serialize() {
  MemoryBuffer *info_fragment = new MemoryBuffer(sizeof(write_request_t));
  write_request_t *request = (write_request_t *)info_fragment->GetBuffer();

  request->points_count = this->points_count;
  memcpy(request->series_name, this->series_name.c_str(), this->series_name.size());

  ShallowBuffer *points_fragment = new ShallowBuffer(
      (byte_t *)this->points,
      this->points_count * sizeof(data_point_t));

  return std::vector<Buffer *> {info_fragment, points_fragment};
}

}