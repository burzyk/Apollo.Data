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
// Created by Pawel Burzynski on 14/04/2017.
//

#include "src/utils/network.h"

#include <sys/socket.h>
#include <libdill.h>

sdb_socket_t sdb_socket_listen(int port, int backlog) {
  struct ipaddr addr;
  sdb_socket_t sock;

  if (ipaddr_local(&addr, NULL, port, 0) < 0) {
    return SDB_INVALID_SOCKET;
  }

  if ((sock = tcp_listen(&addr, backlog)) < 0) {
    return SDB_INVALID_SOCKET;
  }

  return sock;
}

sdb_socket_t sdb_socket_connect(const char *hostname, int port) {
  struct ipaddr addr;

  if (ipaddr_remote(&addr, hostname, port, 0, -1) < 0) {
    return SDB_INVALID_SOCKET;
  }

  return tcp_connect(&addr, -1);
}

int sdb_socket_receive(sdb_socket_t socket, void *buffer, size_t size) {
  return brecv(socket, buffer, size, -1) < 0 ? 0 : (int)size;
}

int sdb_socket_send(sdb_socket_t socket, void *buffer, size_t size) {
  if (buffer == NULL) {
    return 0;
  }

  return bsend(socket, buffer, size, -1) < 0 ? 0 : (int)size;
}

void sdb_socket_close(sdb_socket_t socket) {
  tcp_close(socket, -1);
}

sdb_socket_t sdb_socket_accept(sdb_socket_t socket) {
  return tcp_accept(socket, NULL, now() + 1000);
}
