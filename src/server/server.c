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

#include "src/server/server.h"
#include "src/utils/memory.h"
#include "src/utils/diagnostics.h"

void *sdb_server_worker_routine(void *data);
void sdb_server_handle_read(sdb_server_t *server, sdb_socket_t client_socket, sdb_packet_t *packet);
void sdb_server_handle_write(sdb_server_t *server, sdb_socket_t client_socket, sdb_packet_t *packet);

sdb_server_t *sdb_server_create(int port, int backlog, int max_clients, int points_per_packet, sdb_database_t *db) {
  sdb_server_t *server = (sdb_server_t *)sdb_alloc(sizeof(sdb_server_t));
  server->_db = db;
  server->_is_running = 1;
  server->_master_socket = sdb_socket_listen(port, backlog);
  server->_points_per_packet = points_per_packet;
  server->_thread_pool_size = max_clients;
  server->_thread_pool = (sdb_thread_t **)sdb_alloc(sizeof(sdb_thread_t *) * server->_thread_pool_size);

  if (server->_master_socket == SDB_INVALID_SOCKET) {
    die("Unable to listen");
  }

  for (int i = 0; i < server->_thread_pool_size; i++) {
    server->_thread_pool[i] = sdb_thread_start(sdb_server_worker_routine, server);
  }

  return server;
}

void sdb_server_destroy(sdb_server_t *server) {
  server->_is_running = 0;
  sdb_socket_close(server->_master_socket);

  for (int i = 0; i < server->_thread_pool_size; i++) {
    sdb_thread_join_and_destroy(server->_thread_pool[i]);
  }

  sdb_free(server->_thread_pool);
  sdb_free(server);
}

void *sdb_server_worker_routine(void *data) {
  sdb_server_t *server = (sdb_server_t *)data;

  while (server->_is_running) {
    int client_socket;

    if ((client_socket = sdb_socket_accept(server->_master_socket)) != SDB_INVALID_SOCKET) {
      sdb_packet_t *packet = NULL;

      while ((packet = sdb_packet_receive(client_socket)) != NULL) {
        switch (packet->header.type) {
          case SDB_READ_REQUEST: sdb_server_handle_read(server, client_socket, packet);
            break;
          case SDB_WRITE_REQUEST:sdb_server_handle_write(server, client_socket, packet);
            break;
          default: sdb_log_info("Unknown packet type");
        }

        sdb_packet_destroy(packet);
      }

      sdb_socket_close(client_socket);
    }
  }

  return NULL;
}

void sdb_server_handle_read(sdb_server_t *server, sdb_socket_t client_socket, sdb_packet_t *packet) {
  sdb_read_request_t *request = (sdb_read_request_t *)packet->payload;
  sdb_timestamp_t begin = request->begin;

  for (;;) {
    sdb_data_points_reader_t *reader =
        sdb_database_read(server->_db, request->data_series_id, begin, request->end, server->_points_per_packet);

    int skip = begin == request->begin || reader->points_count == 0 ? 0 : 1;
    int sent_points_count = reader->points_count - skip;
    sdb_data_point_t *points = &reader->points[skip];

    int send_status = sdb_packet_send_and_destroy(
        sdb_read_response_create(SDB_RESPONSE_OK, points, sent_points_count),
        client_socket);

    begin = reader->points[reader->points_count - 1].time;
    sdb_data_points_reader_destroy(reader);

    if (!send_status) {
      return;
    }

    if (sent_points_count == 0) {
      break;
    }
  }
}

void sdb_server_handle_write(sdb_server_t *server, sdb_socket_t client_socket, sdb_packet_t *packet) {
  sdb_write_request_t *request = (sdb_write_request_t *)packet->payload;
  int status = sdb_database_write(server->_db, request->data_series_id, request->points, request->points_count);

  sdb_packet_send_and_destroy(
      sdb_write_response_create(status ? SDB_RESPONSE_ERROR : SDB_RESPONSE_OK),
      client_socket);
}
