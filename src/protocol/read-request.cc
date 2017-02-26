//
// Created by Pawel Burzynski on 19/02/2017.
//

#include <string.h>
#include <src/utils/memory-buffer.h>
#include <src/fatal-exception.h>
#include "read-request.h"

namespace shakadb {

ReadRequest::ReadRequest() : ReadRequest("", data_point_t::kMinTimestamp, data_point_t::kMinTimestamp) {
}

ReadRequest::ReadRequest(std::string series_name, timestamp_t begin, timestamp_t end) {
  if (series_name.size() > SHAKADB_SERIES_NAME_MAX_LENGTH) {
    throw FatalException("series name is too long");
  }

  this->series_name = series_name;
  this->begin = begin;
  this->end = end;
}

PacketType ReadRequest::GetType() {
  return kReadRequest;
}

std::string ReadRequest::GetSeriesName() {
  return this->series_name;
}

timestamp_t ReadRequest::GetBegin() {
  return this->begin;
}

timestamp_t ReadRequest::GetEnd() {
  return this->end;
}

bool ReadRequest::Deserialize(Buffer *payload) {
  if (payload->GetSize() != sizeof(read_request_t)) {
    return false;
  }

  read_request_t *request = (read_request_t *)payload->GetBuffer();

  this->begin = request->begin;
  this->end = request->end;
  this->series_name = std::string(request->series_name);

  return true;
}

std::vector<Buffer *> ReadRequest::Serialize() {
  MemoryBuffer *buffer = new MemoryBuffer(sizeof(read_request_t));
  read_request_t *request = (read_request_t *)buffer->GetBuffer();

  request->begin = this->begin;
  request->end = this->end;
  memcpy(request->series_name, this->series_name.c_str(), this->series_name.size());

  return std::vector<Buffer *> {buffer};
}

}
