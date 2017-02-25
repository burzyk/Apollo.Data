//
// Created by Pawel Burzynski on 19/02/2017.
//

#include <src/utils/memory-buffer.h>
#include "read-request.h"

namespace shakadb {

ReadRequest::ReadRequest() : ReadRequest("", data_point_t::kMinTimestamp, data_point_t::kMinTimestamp) {
}

ReadRequest::ReadRequest(std::string series_name, timestamp_t begin, timestamp_t end) {
  this->series_name = series_name;
  this->begin = begin;
  this->end = end;

//  MemoryBuffer *payload = new MemoryBuffer(sizeof(int) + series_name.size() + 2 * sizeof(timestamp_t));
//  int series_name_size

//  memcpy(this->GetPayload(), &series_name_size, sizeof(int));
//  memcpy(this->GetPayload() + sizeof(int), series_name.c_str(), series_name.size());
//  memcpy(this->GetPayload() + sizeof(int) + series_name.size(), &begin, sizeof(timestamp_t));
//  memcpy(this->GetPayload() + sizeof(int) + series_name.size() + sizeof(timestamp_t), &end, sizeof(timestamp_t));
}

PacketType ReadRequest::GetType() {
  return kReadRequest;
}

std::string ReadRequest::GetSeriesName() {
  this->series_name;
}

timestamp_t ReadRequest::GetBegin() {
  this->begin;
}

timestamp_t ReadRequest::GetEnd() {
  this->end;
}

void ReadRequest::Deserialize(Buffer *payload) {

}

std::vector<Buffer *> ReadRequest::Serialize() {
  return std::vector<Buffer *>();
}

}