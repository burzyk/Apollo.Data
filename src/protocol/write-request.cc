//
// Created by Pawel Burzynski on 16/02/2017.
//

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

void WriteRequest::Deserialize(Buffer *payload) {

}

std::vector<Buffer *> WriteRequest::Serialize() {
  return std::vector<Buffer *>();
}

}