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

#ifndef SRC_PROTOCOL_H_
#define SRC_PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>

#include "src/common.h"

#include "src/utils/network.h"

typedef char sdb_packet_type_t;
typedef char sdb_response_code_t;

#define SDB_RESPONSE_OK ((sdb_response_code_t)0)
#define SDB_RESPONSE_ERROR  ((sdb_response_code_t)1)

#define SDB_WRITE_REQUEST ((sdb_packet_type_t)1)
#define SDB_READ_REQUEST ((sdb_packet_type_t)3)
#define SDB_READ_RESPONSE  ((sdb_packet_type_t)4)
#define SDB_SIMPLE_RESPONSE  ((sdb_packet_type_t)2)
#define SDB_TRUNCATE_REQUEST  ((sdb_packet_type_t)5)
#define SDB_READ_LATEST_REQUEST ((sdb_packet_type_t)6)

#define SDB_PACKET_MAX_LEN  65536000

typedef struct sdb_packet_header_s {
  sdb_packet_type_t type;
  size_t payload_size;
} sdb_packet_header_t;

typedef struct sdb_packet_s {
  sdb_packet_header_t header;
  void *payload;
  void *_raw_payload;
} sdb_packet_t;

typedef struct sdb_write_request_s {
  sdb_data_series_id_t data_series_id;
  int points_count;
  sdb_data_point_t *points;
} sdb_write_request_t;

typedef struct sdb_read_request_s {
  sdb_data_series_id_t data_series_id;
  sdb_timestamp_t begin;
  sdb_timestamp_t end;
  int points_per_packet;
} sdb_read_request_t;

typedef struct sdb_read_latest_request_s {
  sdb_data_series_id_t data_series_id;
} sdb_read_latest_request_t;

typedef struct sdb_read_response_s {
  sdb_response_code_t code;
  int points_count;
  sdb_data_point_t *points;
} sdb_read_response_t;

typedef struct sdb_truncate_request_s {
  sdb_data_series_id_t data_series_id;
} sdb_truncate_request_t;

typedef struct sdb_simple_response_s {
  sdb_response_code_t code;
} sdb_simple_response_t;

sdb_packet_t *sdb_write_request_create(sdb_data_series_id_t data_series_id, sdb_data_point_t *points, int points_count);
sdb_packet_t *sdb_read_request_create(sdb_data_series_id_t data_series_id,
                                      sdb_timestamp_t begin,
                                      sdb_timestamp_t end,
                                      int points_per_packet);
sdb_packet_t *sdb_read_latest_request_create(sdb_data_series_id_t data_series_id);
sdb_packet_t *sdb_read_response_create(sdb_response_code_t code, sdb_data_point_t *points, int points_count);
sdb_packet_t *sdb_simple_response_create(sdb_response_code_t code);
sdb_packet_t *sdb_truncate_request_create(sdb_data_series_id_t data_series_id);

sdb_packet_t *sdb_packet_receive(sdb_socket_t socket);
int sdb_packet_send(sdb_packet_t *packet, sdb_socket_t socket);
int sdb_packet_send_and_destroy(sdb_packet_t *packet, sdb_socket_t socket);
void sdb_packet_destroy(sdb_packet_t *packet);

#endif  // SRC_PROTOCOL_H_
