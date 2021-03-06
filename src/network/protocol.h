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
#include "src/data-point.h"

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

// spells 'KAMA' in ASCII
#define SDB_SERVER_MAGIC 0x4B414D41

// defines the transport layer frame
typedef struct packet_s {
  uint32_t magic;
  uint32_t total_size;
  uint8_t payload[];
} __attribute__((packed)) packet_t;

// defines the application layer payload
typedef struct payload_header_s {
  packet_type_t type;
} __attribute__((packed)) payload_header_t;

typedef struct write_request_s {
  payload_header_t header;
  series_id_t data_series_id;
  uint64_t points_count;
  uint32_t point_size;
  data_point_t points[];
} __attribute__((packed)) write_request_t;

typedef struct read_request_s {
  payload_header_t header;
  series_id_t data_series_id;
  timestamp_t begin;
  timestamp_t end;
} __attribute__((packed)) read_request_t;

typedef struct read_latest_request_s {
  payload_header_t header;
  series_id_t data_series_id;
} __attribute__((packed)) read_latest_request_t;

typedef struct read_response_s {
  payload_header_t header;
  uint64_t points_count;
  uint32_t point_size;
  data_point_t points[];
} __attribute__((packed)) read_response_t;

typedef struct truncate_request_s {
  payload_header_t header;
  series_id_t data_series_id;
} __attribute__((packed)) truncate_request_t;

typedef struct simple_response_s {
  payload_header_t header;
  response_code_t code;
} __attribute__((packed)) simple_response_t;

buffer_t write_request_create(series_id_t data_series_id, points_list_t *points);
buffer_t read_request_create(series_id_t data_series_id, timestamp_t begin, timestamp_t end);
buffer_t read_latest_request_create(series_id_t data_series_id);
buffer_t read_response_create(points_list_t *points);
buffer_t simple_response_create(response_code_t code);
buffer_t truncate_request_create(series_id_t data_series_id);

int payload_validate(uint8_t *data, size_t size);

#endif  // SRC_NETWORK_PROTOCOL_H_
