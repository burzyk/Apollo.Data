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
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <signal.h>

#include "src/common.h"

sdb_socket_t sdb_socket_create(int family, int type, int protocol);
void sdb_socket_init(sdb_socket_t sock);

sdb_socket_t sdb_socket_listen(int port, int backlog) {
  sdb_socket_t sock;

  if ((sock = sdb_socket_create(AF_INET, SOCK_STREAM, 0)) == -1) {
    return -1;
  }

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr("0.0.0.0");

  if (bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    return -1;
  }

  if (listen(sock, backlog) == -1) {
    return -1;
  }

  return sock;
}

sdb_socket_t sdb_socket_connect(const char *hostname, int port) {
  struct addrinfo hints = {0};
  struct addrinfo *result;
  sdb_socket_t sock = -1;
  char port_string[SDB_FILE_MAX_LEN] = {0};
  snprintf(port_string, SDB_FILE_MAX_LEN, "%d", port);

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(hostname, port_string, &hints, &result) != 0) {
    return sock;
  }

  for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
    if ((sock = sdb_socket_create(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
      continue;
    }

    if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) {
      break;
    }

    close(sock);
    sock = -1;
  }

  freeaddrinfo(result);
  return sock;
}

int sdb_socket_receive(sdb_socket_t socket, void *buffer, size_t size) {
  ssize_t read = 0;
  int total_read = 0;
  char *ptr = (char *)buffer;

  while (size && (read = recv(socket, ptr, size, 0)) > 0) {
    ptr += read;
    size -= read;
    total_read += read;
  }

  return total_read;
}

int sdb_socket_send(sdb_socket_t socket, void *buffer, size_t size) {
  ssize_t sent = 0;
  int total_send = 0;
  char *ptr = (char *)buffer;

  while (size && (sent = send(socket, ptr, size, 0)) > 0) {
    ptr += sent;
    size -= sent;
    total_send += sent;
  }

  return total_send;
}

void sdb_socket_close(sdb_socket_t socket) {
  shutdown(socket, SHUT_RDWR);
  close(socket);
}

sdb_socket_t sdb_socket_accept(sdb_socket_t socket) {
  sdb_socket_t sock = accept(socket, NULL, NULL);
  sdb_socket_init(sock);

  return sock;
}

sdb_socket_t sdb_socket_create(int family, int type, int protocol) {
  signal(SIGPIPE, SIG_IGN);
  sdb_socket_t sock = socket(family, type, protocol);
  sdb_socket_init(sock);

  return sock;
}

void sdb_socket_init(sdb_socket_t sock) {
  if (sock == SDB_INVALID_SOCKET) {
    return;
  }

  int option_value = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &option_value, sizeof(option_value)) < 0) {
    die("unable to set SO_NOSIGPIPE on a socket");
  }
}