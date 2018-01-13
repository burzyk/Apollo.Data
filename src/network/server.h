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

#ifndef SRC_NETWORK_SERVER_H_
#define SRC_NETWORK_SERVER_H_

#include <uv.h>

#ifndef SDB_MAX_CLIENTS
#define SDB_MAX_CLIENTS 255
#endif

#ifndef SDB_SERVER_PACKET_MAX_LEN
#define SDB_SERVER_PACKET_MAX_LEN 6553600
#endif

#define SDB_SERVER_READ_BUFFER_MAX_LEN  65536

// spells 'KAMA' in ASCII
#define SDB_SERVER_MAGIC 0x4B414D41

typedef struct header_s {
  uint32_t magic;
  uint32_t total_size;
  uint8_t payload[];
} header_t;

typedef struct client_s {
  uv_tcp_t socket;
  struct server_s *server;
  int index;

  // as the SDB_SERVER_READ_BUFFER_MAX_LEN is maximal value read can return
  // we can be certain that at any given time we will not need more memory
  // than SDB_SERVER_PACKET_MAX_LEN + SDB_SERVER_READ_BUFFER_MAX_LEN
  uint8_t buffer[SDB_SERVER_PACKET_MAX_LEN + SDB_SERVER_READ_BUFFER_MAX_LEN];
  size_t buffer_length;
} client_t;

typedef int (*packet_handler_t)(client_t *client, uint8_t *data, uint32_t size, void *context);

typedef struct server_s {
  int _port;
  uv_loop_t *_loop;
  uv_tcp_t _master_socket;
  client_t *_clients[SDB_MAX_CLIENTS];
  packet_handler_t _handler;
  void *_handler_context;
} server_t;

server_t *server_create(int port, packet_handler_t handler, void *handler_context);
void server_run(server_t *server);
void server_stop(server_t *server);
void server_destroy(server_t *server);

#endif  // SRC_NETWORK_SERVER_H_
