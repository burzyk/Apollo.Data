//
// Created by Pawel Burzynski on 17/02/2017.
//

#include <src/fatal-exception.h>
#include <src/utils/allocator.h>
#include <sys/socket.h>
#include <src/utils/common.h>
#include "receive-stream.h"

namespace shakadb {

ReceiveStream::ReceiveStream(int socket, int received_buffer_grow) {
  this->socket = socket;
  this->total_recv_buffer = new RingBuffer(received_buffer_grow);
  this->current_recv_buffer = Allocator::New<uint8_t>(SHAKA_CURRENT_BUFFER_SIZE);
}

ReceiveStream::~ReceiveStream() {
  delete this->total_recv_buffer;
  delete this->current_recv_buffer;
}

int ReceiveStream::Read(uint8_t *buffer, int buffer_size) {
  this->LoadMissing(buffer_size);
  return this->total_recv_buffer->Read(buffer, buffer_size);
}

int ReceiveStream::Peek(uint8_t *buffer, int buffer_size) {
  this->LoadMissing(buffer_size);
  return this->total_recv_buffer->Read(buffer, buffer_size);
}

void ReceiveStream::Write(uint8_t *buffer, int buffer_size) {
  throw FatalException("This method is not supported");
}

bool ReceiveStream::HasData(int size) {
  this->LoadMissing(size);
  return this->total_recv_buffer->HasData(size);
}

void ReceiveStream::LoadMissing(int requested_size) {
  int read = 0;
  requested_size = MAX(requested_size - this->total_recv_buffer->GetSize(), 0);

  do {
    requested_size -= read;
    int to_read = MIN(SHAKA_CURRENT_BUFFER_SIZE, requested_size);
    this->total_recv_buffer->Write(this->current_recv_buffer, read);
    read = recv(this->socket, this->current_recv_buffer, to_read, 0);
  } while (read > 0 && requested_size > 0);
}

}