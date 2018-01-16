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
// Created by Pawel Burzynski on 08/02/2017.
//

#include "src/client/session.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <memory.h>

int socket_connect(const char *server, int port);
void socket_send(int socket, uint8_t *buffer, size_t size);
int socket_receive(int socket, void *buffer, size_t size);
void socket_close(int socket);

int session_send_with_simple_response(session_t *session, buffer_t request);
void *session_receive(session_t *session, packet_type_t type);
void session_send_and_destroy(session_t *session, buffer_t response);

int socket_connect(const char *server, int port) {
  struct addrinfo hints = {0};
  struct addrinfo *result;
  int sock = -1;
  char port_string[SDB_FILE_MAX_LEN] = {0};
  snprintf(port_string, SDB_FILE_MAX_LEN, "%d", port);

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(server, port_string, &hints, &result) != 0) {
    return sock;
  }

  for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
    if ((sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
      continue;
    } else {
#ifdef __APPLE__
      int option_value = 1;
      if (setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &option_value, sizeof(option_value)) < 0) {
        die("unable to set SO_NOSIGPIPE on a socket");
      }
#endif
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

void socket_send(int socket, uint8_t *buffer, size_t size) {
  if (buffer == NULL) {
    return;
  }

  ssize_t sent = 0;
  int total_send = 0;
  char *ptr = (char *)buffer;

  while (size && (sent = send(socket, ptr, size, 0)) > 0) {
    ptr += sent;
    size -= sent;
    total_send += sent;
  }
}

int socket_receive(int socket, void *buffer, size_t size) {
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

void socket_close(int socket) {
  shutdown(socket, SHUT_RDWR);
  close(socket);
}

session_t *session_create(const char *server, int port) {
  int sock = socket_connect(server, port);

  if (sock == -1) {
    return NULL;
  }

  session_t *session = (session_t *)sdb_alloc(sizeof(session_t));
  session->socket = sock;
  session->read_response = NULL;
  session->read_open = 0;

  return session;
}

void session_destroy(session_t *session) {
  if (session->read_response != NULL) {
    sdb_free(session->read_response);
  }

  socket_close(session->socket);
  sdb_free(session);
}

int session_write(session_t *session, series_id_t series_id, data_point_t *points, int count) {
  if (session->read_open) {
    return -1;
  }

  buffer_t request = write_request_create(series_id, points, count);
  return session_send_with_simple_response(session, request);
}

int session_truncate(session_t *session, series_id_t series_id) {
  if (session->read_open) {
    return -1;
  }

  buffer_t request = truncate_request_create(series_id);
  return session_send_with_simple_response(session, request);
}

int session_read(session_t *session,
                 series_id_t series_id,
                 timestamp_t begin,
                 timestamp_t end,
                 int points_per_packet) {
  if (session->read_open) {
    return -1;
  }

  buffer_t packet = read_request_create(series_id, begin, end, points_per_packet);
  session_send_and_destroy(session, packet);
  session->read_open = 1;

  return 0;
}

int session_read_next(session_t *session) {
  if (!session->read_open) {
    return 0;
  }

  if (session->read_response != NULL) {
    sdb_free(session->read_response);
    session->read_response = NULL;
  }

  read_response_t *response = (read_response_t *)session_receive(session, SDB_READ_RESPONSE);

  if (response == NULL || response->points_count == 0) {
    sdb_free(response);
    session->read_open = 0;
    return 0;
  }

  session->read_response = response;
  return 1;
}

int session_read_latest(session_t *session, series_id_t series_id, data_point_t *latest) {
  if (session->read_open) {
    return -1;
  }

  buffer_t packet = read_latest_request_create(series_id);
  session_send_and_destroy(session, packet);

  read_response_t *response = (read_response_t *)session_receive(session, SDB_READ_RESPONSE);

  latest->time = response->points_count != 0 ? response->points[0].time : 0;
  latest->value = response->points_count != 0 ? response->points[0].value : 0;

  sdb_free(response);

  return 0;
}

int session_send_with_simple_response(session_t *session, buffer_t request) {
  session_send_and_destroy(session, request);
  simple_response_t *response = (simple_response_t *)session_receive(session, SDB_SIMPLE_RESPONSE);

  int result = response == NULL || response->code != SDB_RESPONSE_OK;
  sdb_free(response);

  return result;
}

void session_send_and_destroy(session_t *session, buffer_t response) {
  packet_t hdr;
  hdr.magic = SDB_SERVER_MAGIC;
  hdr.total_size = sizeof(packet_t) + response.size;

  socket_send(session->socket, (uint8_t *)&hdr, sizeof(hdr));
  socket_send(session->socket, response.content, response.size);

  sdb_free(response.content);
}

void *session_receive(session_t *session, packet_type_t type) {
  packet_t header;

  if (socket_receive(session->socket, &header, sizeof(header)) != sizeof(header)) {
    return NULL;
  }

  if (header.magic != SDB_SERVER_MAGIC) {
    return NULL;
  }

  size_t payload_size = header.total_size - sizeof(packet_t);
  void *payload = sdb_alloc(payload_size);

  if (socket_receive(session->socket, payload, payload_size) != payload_size) {
    sdb_free(payload);
    return NULL;
  }

  payload_header_t *hdr = (payload_header_t *)payload;

  if (hdr->type != type) {
    sdb_free(payload);
    return NULL;
  }

  if (!payload_validate(payload, payload_size)) {
    sdb_free(payload);
    return NULL;
  }

  return payload;
}
