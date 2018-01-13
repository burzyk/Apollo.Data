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

#ifndef SRC_SERVER_SERVER_H__
#define SRC_SERVER_SERVER_H__

#include <uv.h>

#include "src/storage/database.h"
#include "src/protocol.h"

#ifndef SDB_MAX_CLIENTS
#define SDB_MAX_CLIENTS 255
#endif

typedef struct client_s {
  uv_tcp_t socket;
  uint8_t buffer[SDB_PACKET_MAX_LEN];
  int buffer_length;
  struct server_s *server;
  int index;
} client_t;

typedef struct server_s {
  sdb_database_t *_db;
  int _port;
  volatile int _stop;
  uv_loop_t *_loop;
  uv_tcp_t _master_socket;
  client_t *_clients[SDB_MAX_CLIENTS];
} server_t;

server_t *server_create(int port, sdb_database_t *db);
void server_run(server_t *server);
void server_stop(server_t *server);
void server_destroy(server_t *server);

#endif  // SRC_SERVER_SERVER_H__
