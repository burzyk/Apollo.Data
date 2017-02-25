//
// Created by Pawel Burzynski on 19/02/2017.
//

#include <src/utils/memory-buffer.h>
#include "read-request.h"

namespace shakadb {

ReadRequest::ReadRequest(std::string series_name, timestamp_t begin, timestamp_t end)
    : DataPacket(kReadRequest, sizeof(int) + series_name.size() + 2 * sizeof(timestamp_t)) {
//  MemoryBuffer *payload = new MemoryBuffer(sizeof(int) + series_name.size() + 2 * sizeof(timestamp_t));
//  int series_name_size

//  memcpy(this->GetPayload(), &series_name_size, sizeof(int));
//  memcpy(this->GetPayload() + sizeof(int), series_name.c_str(), series_name.size());
//  memcpy(this->GetPayload() + sizeof(int) + series_name.size(), &begin, sizeof(timestamp_t));
//  memcpy(this->GetPayload() + sizeof(int) + series_name.size() + sizeof(timestamp_t), &end, sizeof(timestamp_t));
}

ReadRequest::ReadRequest(Buffer *packet)
    : DataPacket(packet) {
}

PacketType ReadRequest::GetType() {
  return kReadRequest;
}

std::string ReadRequest::GetSeriesName() {
}

timestamp_t ReadRequest::GetBegin() {
}

timestamp_t ReadRequest::GetEnd() {
}

int ReadRequest::GetSeriesNameSize() {
}

}