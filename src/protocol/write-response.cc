//
// Created by Pawel Burzynski on 25/02/2017.
//

#include <src/utils/memory-buffer.h>
#include "write-response.h"

namespace shakadb {

WriteResponse::WriteResponse() {

}

WriteResponse::WriteResponse(WriteStatus status) {
  this->status = status;
}

PacketType WriteResponse::GetType() {
  return kWriteResponse;
}

WriteStatus WriteResponse::GetStatusCount() {
  return this->status;
}

bool WriteResponse::Deserialize(Buffer *payload) {
  if (payload->GetSize() != sizeof(write_response_t)) {
    return false;
  }

  write_response_t *response = (write_response_t *)payload->GetBuffer();

  this->status = response->status;

  return true;
}

std::vector<Buffer *> WriteResponse::Serialize() {
  MemoryBuffer *buffer = new MemoryBuffer(sizeof(write_response_t));
  write_response_t *response = (write_response_t *)buffer->GetBuffer();

  response->status = this->status;

  return std::vector<Buffer *> {buffer};
}

}