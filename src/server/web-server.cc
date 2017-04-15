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

#include "src/server/web-server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <src/utils/memory.h>
#include <src/storage/data-points-reader.h>
#include <src/storage/database.h>
#include <src/utils/diagnostics.h>

namespace shakadb {

WebServer::WebServer(int port, int backlog, int thread_pool_size, int points_per_packet, sdb_database_t *db) {
  this->port = port;
  this->backlog = backlog;
  this->is_running = false;
  this->master_socket = -1;
  this->db = db;
  this->points_per_packet = points_per_packet;
  this->thread_pool_size = thread_pool_size;
  this->thread_pool = NULL;
}

WebServer::~WebServer() {
  this->is_running = false;
  sdb_socket_close(this->master_socket);

  for (int i = 0; i < this->thread_pool_size; i++) {
    sdb_thread_join_and_destroy(this->thread_pool[i]);
  }
}

void WebServer::Listen() {
  this->master_socket = sdb_socket_listen(this->port, this->backlog);

  if (this->master_socket == -1) {
    die("Unable to listen");
  }

  this->is_running = true;
  this->thread_pool = (sdb_thread_t **)sdb_alloc(this->thread_pool_size * sizeof(sdb_thread_t *));

  for (int i = 0; i < this->thread_pool_size; i++) {
    sdb_thread_start(this->thread_pool[i], (sdb_thread_routine_t)WorkerRoutine, this);
  }
}

void WebServer::WorkerRoutine(void *data) {
  WebServer *_this = (WebServer *)data;

  while (_this->is_running) {
    int client_socket;

    if ((client_socket = sdb_socket_accept(_this->master_socket)) != -1) {
      sdb_packet_t *packet = NULL;

      while ((packet = sdb_packet_receive(client_socket)) != NULL) {
        switch (packet->header.type) {
          case SDB_READ_REQUEST:_this->HandleRead(client_socket, packet);
            break;
          case SDB_WRITE_REQUEST:_this->HandleWrite(client_socket, packet);
            break;
          default: sdb_log_info("Unknown packet type");
        }

        sdb_packet_destroy(packet);
      }

      sdb_socket_close(client_socket);
    }
  }
}

void WebServer::HandleRead(sdb_socket_t client_socket, sdb_packet_t *packet) {
  sdb_read_request_t *request = (sdb_read_request_t *)packet->payload;
  sdb_timestamp_t begin = request->begin;

  while (true) {
    sdb_data_points_reader_t *reader =
        sdb_database_read(this->db, request->data_series_id, begin, request->end, this->points_per_packet);

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

void WebServer::HandleWrite(sdb_socket_t client_socket, sdb_packet_t *packet) {
  sdb_write_request_t *request = (sdb_write_request_t *)packet->payload;
  int status = sdb_database_write(this->db, request->data_series_id, request->points, request->points_count);

  sdb_packet_send_and_destroy(
      sdb_write_response_create(status ? SDB_RESPONSE_ERROR : SDB_RESPONSE_OK),
      client_socket);
}

}  // namespace shakadb
