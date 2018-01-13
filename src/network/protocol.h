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

#ifndef SRC_NETWORK_PROTOCOL_H_
#define SRC_NETWORK_PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>

#include "src/common.h"

typedef char packet_type_t;
typedef char response_code_t;

#define SDB_RESPONSE_OK ((response_code_t)0)
#define SDB_RESPONSE_ERROR  ((response_code_t)1)

#define SDB_WRITE_REQUEST ((packet_type_t)1)
#define SDB_READ_REQUEST ((packet_type_t)3)
#define SDB_READ_RESPONSE  ((packet_type_t)4)
#define SDB_SIMPLE_RESPONSE  ((packet_type_t)2)
#define SDB_TRUNCATE_REQUEST  ((packet_type_t)5)
#define SDB_READ_LATEST_REQUEST ((packet_type_t)6)

typedef struct packet_header_s {
  packet_type_t type;
} packet_header_t;

typedef struct packet_s {
  void *payload;
  size_t size;
} packet_t;

typedef struct write_request_s {
  packet_header_t header;
  sdb_data_series_id_t data_series_id;
  int points_count;
  sdb_data_point_t *points;
} write_request_t;

typedef struct read_request_s {
  packet_header_t header;
  sdb_data_series_id_t data_series_id;
  sdb_timestamp_t begin;
  sdb_timestamp_t end;
  int points_per_packet;
} read_request_t;

typedef struct read_latest_request_s {
  packet_header_t header;
  sdb_data_series_id_t data_series_id;
} read_latest_request_t;

typedef struct read_response_s {
  packet_header_t header;
  response_code_t code;
  int points_count;
  sdb_data_point_t points[];
} read_response_t;

typedef struct truncate_request_s {
  packet_header_t header;
  sdb_data_series_id_t data_series_id;
} truncate_request_t;

typedef struct simple_response_s {
  packet_header_t header;
  response_code_t code;
} simple_response_t;

packet_t write_request_create(sdb_data_series_id_t data_series_id, sdb_data_point_t *points, int points_count);
packet_t read_request_create(sdb_data_series_id_t data_series_id,
                             sdb_timestamp_t begin,
                             sdb_timestamp_t end,
                             int points_per_packet);
packet_t read_latest_request_create(sdb_data_series_id_t data_series_id);
packet_t read_response_create(response_code_t code, sdb_data_point_t *points, int points_count);
packet_t simple_response_create(response_code_t code);
packet_t truncate_request_create(sdb_data_series_id_t data_series_id);

int packet_validate(uint8_t *data, int size);

#endif  // SRC_NETWORK_PROTOCOL_H_
