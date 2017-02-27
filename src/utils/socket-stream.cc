/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
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