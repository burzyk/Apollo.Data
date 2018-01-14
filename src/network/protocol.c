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

#include "src/utils/memory.h"

packet_t write_request_create(sdb_data_series_id_t data_series_id, sdb_data_point_t *points, int points_count) {
  size_t total_size = sizeof(write_request_t) + points_count * sizeof(sdb_data_point_t);
  write_request_t *request = (write_request_t *)sdb_alloc(total_size);
  request->header.type = SDB_WRITE_REQUEST;
  request->data_series_id = data_series_id;
  request->points_count = points_count;
  memcpy(request->points, points, points_count * sizeof(sdb_data_point_t));

  packet_t packet;
  packet.payload = request;
  packet.size = total_size;

  return packet;
}

packet_t simple_response_create(response_code_t code) {
  simple_response_t *response = (simple_response_t *)sdb_alloc(sizeof(simple_response_t));
  response->header.type = SDB_SIMPLE_RESPONSE;
  response->code = code;

  packet_t packet;
  packet.payload = response;
  packet.size = sizeof(simple_response_t);

  return packet;
}

packet_t read_request_create(sdb_data_series_id_t data_series_id,
                             sdb_timestamp_t begin,
                             sdb_timestamp_t end,
                             int points_per_packet) {
  read_request_t *request = (read_request_t *)sdb_alloc(sizeof(read_request_t));
  request->header.type = SDB_READ_REQUEST;
  request->data_series_id = data_series_id;
  request->begin = begin;
  request->end = end;
  request->points_per_packet = points_per_packet;

  packet_t packet;
  packet.payload = request;
  packet.size = sizeof(read_request_t);

  return packet;
}

packet_t read_latest_request_create(sdb_data_series_id_t data_series_id) {
  read_latest_request_t *request = (read_latest_request_t *)sdb_alloc(sizeof(read_latest_request_t));
  request->header.type = SDB_READ_LATEST_REQUEST;
  request->data_series_id = data_series_id;

  packet_t packet;
  packet.payload = request;
  packet.size = sizeof(read_latest_request_t);

  return packet;
}

packet_t read_response_create(response_code_t code, sdb_data_point_t *points, int points_count) {
  size_t total_size = sizeof(read_response_t) + points_count * sizeof(sdb_data_point_t);
  read_response_t *response = (read_response_t *)sdb_alloc(total_size);
  response->header.type = SDB_READ_RESPONSE;
  response->code = code;
  response->points_count = points_count;
  memcpy(response->points, points, points_count * sizeof(sdb_data_point_t));

  packet_t packet;
  packet.payload = response;
  packet.size = total_size;

  return packet;
}

packet_t truncate_request_create(sdb_data_series_id_t data_series_id) {
  truncate_request_t *request = (truncate_request_t *)sdb_alloc(sizeof(truncate_request_t));
  request->header.type = SDB_TRUNCATE_REQUEST;
  request->data_series_id = data_series_id;

  packet_t packet;
  packet.payload = request;
  packet.size = sizeof(truncate_request_t);

  return packet;
}