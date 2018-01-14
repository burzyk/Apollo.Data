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

#include "src/network/protocol.h"

#include <string.h>

#include "src/diagnostics.h"

buffer_t write_request_create(sdb_data_series_id_t data_series_id, data_point_t *points, int points_count) {
  size_t total_size = sizeof(write_request_t) + points_count * sizeof(data_point_t);
  write_request_t *request = (write_request_t *)sdb_alloc(total_size);
  request->header.type = SDB_WRITE_REQUEST;
  request->data_series_id = data_series_id;
  request->points_count = points_count;
  memcpy(request->points, points, points_count * sizeof(data_point_t));

  buffer_t packet;
  packet.content = request;
  packet.size = total_size;

  return packet;
}

buffer_t simple_response_create(response_code_t code) {
  simple_response_t *response = (simple_response_t *)sdb_alloc(sizeof(simple_response_t));
  response->header.type = SDB_SIMPLE_RESPONSE;
  response->code = code;

  buffer_t packet;
  packet.content = response;
  packet.size = sizeof(simple_response_t);

  return packet;
}

buffer_t read_request_create(sdb_data_series_id_t data_series_id,
                             timestamp_t begin,
                             timestamp_t end,
                             int points_per_packet) {
  read_request_t *request = (read_request_t *)sdb_alloc(sizeof(read_request_t));
  request->header.type = SDB_READ_REQUEST;
  request->data_series_id = data_series_id;
  request->begin = begin;
  request->end = end;
  request->points_per_packet = points_per_packet;

  buffer_t packet;
  packet.content = request;
  packet.size = sizeof(read_request_t);

  return packet;
}

buffer_t read_latest_request_create(sdb_data_series_id_t data_series_id) {
  read_latest_request_t *request = (read_latest_request_t *)sdb_alloc(sizeof(read_latest_request_t));
  request->header.type = SDB_READ_LATEST_REQUEST;
  request->data_series_id = data_series_id;

  buffer_t packet;
  packet.content = request;
  packet.size = sizeof(read_latest_request_t);

  return packet;
}

buffer_t read_response_create(data_point_t *points, int points_count) {
  size_t total_size = sizeof(read_response_t) + points_count * sizeof(data_point_t);
  read_response_t *response = (read_response_t *)sdb_alloc(total_size);
  response->header.type = SDB_READ_RESPONSE;
  response->points_count = points_count;
  memcpy(response->points, points, points_count * sizeof(data_point_t));

  buffer_t packet;
  packet.content = response;
  packet.size = total_size;

  return packet;
}

buffer_t truncate_request_create(sdb_data_series_id_t data_series_id) {
  truncate_request_t *request = (truncate_request_t *)sdb_alloc(sizeof(truncate_request_t));
  request->header.type = SDB_TRUNCATE_REQUEST;
  request->data_series_id = data_series_id;

  buffer_t packet;
  packet.content = request;
  packet.size = sizeof(truncate_request_t);

  return packet;
}

int payload_validate(uint8_t *data, size_t size) {
  payload_header_t *hdr = (payload_header_t *)data;

  switch (hdr->type) {
    case SDB_WRITE_REQUEST:
      return size >= sizeof(write_request_t) &&
          ((write_request_t *)data)->points_count <= SDB_POINTS_PER_PACKET_MAX &&
          size == sizeof(write_request_t) + sizeof(data_point_t) * ((write_request_t *)data)->points_count;
    case SDB_READ_REQUEST: return size == sizeof(read_request_t);
    case SDB_READ_RESPONSE:
      return size >= sizeof(read_response_t) &&
          ((read_response_t *)data)->points_count <= SDB_POINTS_PER_PACKET_MAX &&
          size == sizeof(read_response_t) + sizeof(data_point_t) * ((read_response_t *)data)->points_count;
    case SDB_SIMPLE_RESPONSE: return size == sizeof(simple_response_t);
    case SDB_TRUNCATE_REQUEST: return size == sizeof(truncate_request_t);
    case SDB_READ_LATEST_REQUEST: return size == sizeof(read_latest_request_t);
    default:log_error("Unknown packet type: %d", (int)hdr->type);
      return 0;
  }
}
