//
// Created by Pawel Burzynski on 25/02/2017.
//

#include <sys/socket.h>
#include <unistd.h>
#include "socket-stream.h"

namespace shakadb {

SocketStream::SocketStream(int socket) {
  this->socket = socket;
}

SocketStream::~SocketStream() {
  this->Close();
}

int SocketStream::Read(byte_t *buffer, int buffer_size) {
  int read = 0;
  int total_read = 0;

  while ((read = recv(this->socket, buffer, buffer_size, 0)) > 0) {
    buffer += read;
    buffer_size -= read;
    total_read += read;
  }

  return total_read;
}

int SocketStream::Write(byte_t *buffer, int buffer_size) {
  int sent = 0;
  int total_send = 0;

  while ((sent = send(this->socket, buffer, buffer_size, 0)) > 0) {
    buffer += sent;
    buffer_size -= sent;
    total_send += sent;
  }

  return total_send;
}

void SocketStream::Close() {
  shutdown(this->socket, SHUT_RDWR);
  close(this->socket);
}

}