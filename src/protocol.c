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

#include "src/protocol.h"

#include <string.h>

#include "src/utils/memory.h"

sdb_write_request_t *sdb_write_request_deserialize(void *payload, size_t size);
sdb_simple_response_t *sdb_simple_response_deserialize(void *payload, size_t size);
sdb_read_request_t *sdb_read_request_deserialize(void *payload, size_t size);
sdb_read_response_t *sdb_read_response_deserialize(void *payload, size_t size);
sdb_truncate_request_t *sdb_truncate_request_deserialize(void *payload, size_t size);

void sdb_write_request_serialize(sdb_write_request_t *request, sdb_socket_t socket);
void sdb_simple_response_serialize(sdb_simple_response_t *response, sdb_socket_t socket);
void sdb_read_request_serialize(sdb_read_request_t *request, sdb_socket_t socket);
void sdb_read_response_serialize(sdb_read_response_t *response, sdb_socket_t socket);
void sdb_truncate_request_serialize(sdb_truncate_request_t *request, sdb_socket_t socket);

/* ========== write request ========== */

sdb_packet_t *sdb_write_request_create(sdb_data_series_id_t data_series_id,
                                       sdb_data_point_t *points,
                                       int points_count) {
  sdb_packet_t *packet = (sdb_packet_t *)sdb_alloc(sizeof(sdb_packet_t));
  packet->header.type = SDB_WRITE_REQUEST;
  packet->header.payload_size = sizeof(data_series_id) +
      sizeof(points_count) +
      points_count * sizeof(sdb_data_point_t);

  sdb_write_request_t *request = (sdb_write_request_t *)sdb_alloc(sizeof(sdb_write_request_t));
  request->data_series_id = data_series_id;
  request->points_count = points_count;
  request->points = points;

  packet->payload = request;
  return packet;
}

sdb_write_request_t *sdb_write_request_deserialize(void *payload, size_t size) {
  sdb_write_request_t *request = (sdb_write_request_t *)sdb_alloc(sizeof(sdb_write_request_t));

  sdb_binary_reader_t reader = {0};
  sdb_binary_reader_init(&reader, payload, size);

  sdb_binary_reader_read(&reader, &request->data_series_id, sizeof(request->data_series_id));
  sdb_binary_reader_read(&reader, &request->points_count, sizeof(request->points_count));
  sdb_binary_reader_read_pointer(&reader, &request->points, sizeof(sdb_data_point_t) * request->points_count);

  if (!reader.success) {
    sdb_free(request);
    return NULL;
  }

  return request;
}

void sdb_write_request_serialize(sdb_write_request_t *request, sdb_socket_t socket) {
  sdb_socket_send(socket, &request->data_series_id, sizeof(request->data_series_id));
  sdb_socket_send(socket, &request->points_count, sizeof(request->points_count));
  sdb_socket_send(socket, request->points, sizeof(sdb_data_point_t) * request->points_count);
}

/* ========== simple response ========== */

sdb_packet_t *sdb_simple_response_create(sdb_response_code_t code) {
  sdb_packet_t *packet = (sdb_packet_t *)sdb_alloc(sizeof(sdb_packet_t));
  packet->header.type = SDB_SIMPLE_RESPONSE;
  packet->header.payload_size = sizeof(code);

  sdb_simple_response_t *response = (sdb_simple_response_t *)sdb_alloc(sizeof(sdb_simple_response_t));
  response->code = code;

  packet->payload = response;
  return packet;
}

sdb_simple_response_t *sdb_simple_response_deserialize(void *payload, size_t size) {
  sdb_simple_response_t *response = (sdb_simple_response_t *)sdb_alloc(sizeof(sdb_simple_response_t));

  sdb_binary_reader_t reader = {0};
  sdb_binary_reader_init(&reader, payload, size);

  sdb_binary_reader_read(&reader, &response->code, sizeof(response->code));

  if (!reader.success) {
    sdb_free(response);
    return NULL;
  }

  return response;
}

void sdb_simple_response_serialize(sdb_simple_response_t *response, sdb_socket_t socket) {
  sdb_socket_send(socket, &response->code, sizeof(response->code));
}

/* ========== read request ========== */

sdb_packet_t *sdb_read_request_create(sdb_data_series_id_t data_series_id, sdb_timestamp_t begin, sdb_timestamp_t end) {
  sdb_packet_t *packet = (sdb_packet_t *)sdb_alloc(sizeof(sdb_packet_t));
  packet->header.type = SDB_READ_REQUEST;
  packet->header.payload_size = sizeof(data_series_id) +
      sizeof(begin) +
      sizeof(end);

  sdb_read_request_t *request = (sdb_read_request_t *)sdb_alloc(sizeof(sdb_read_request_t));
  request->data_series_id = data_series_id;
  request->begin = begin;
  request->end = end;

  packet->payload = request;
  return packet;
}

sdb_read_request_t *sdb_read_request_deserialize(void *payload, size_t size) {
  sdb_read_request_t *request = (sdb_read_request_t *)sdb_alloc(sizeof(sdb_read_request_t));

  sdb_binary_reader_t reader = {0};
  sdb_binary_reader_init(&reader, payload, size);

  sdb_binary_reader_read(&reader, &request->data_series_id, sizeof(request->data_series_id));
  sdb_binary_reader_read(&reader, &request->begin, sizeof(request->begin));
  sdb_binary_reader_read(&reader, &request->end, sizeof(request->end));

  if (!reader.success) {
    sdb_free(request);
    return NULL;
  }

  return request;
}

void sdb_read_request_serialize(sdb_read_request_t *request, sdb_socket_t socket) {
  sdb_socket_send(socket, &request->data_series_id, sizeof(request->data_series_id));
  sdb_socket_send(socket, &request->begin, sizeof(request->begin));
  sdb_socket_send(socket, &request->end, sizeof(request->end));
}

/* ========== read response ========== */

sdb_packet_t *sdb_read_response_create(sdb_response_code_t code, sdb_data_point_t *points, int points_count) {
  sdb_packet_t *packet = (sdb_packet_t *)sdb_alloc(sizeof(sdb_packet_t));
  packet->header.type = SDB_READ_RESPONSE;
  packet->header.payload_size = sizeof(code) +
      sizeof(points_count) +
      points_count * sizeof(sdb_data_point_t);

  sdb_read_response_t *request = (sdb_read_response_t *)sdb_alloc(sizeof(sdb_read_response_t));
  request->code = code;
  request->points_count = points_count;
  request->points = points;

  packet->payload = request;
  return packet;
}

sdb_read_response_t *sdb_read_response_deserialize(void *payload, size_t size) {
  sdb_read_response_t *response = (sdb_read_response_t *)sdb_alloc(sizeof(sdb_read_response_t));

  sdb_binary_reader_t reader = {0};
  sdb_binary_reader_init(&reader, payload, size);

  sdb_binary_reader_read(&reader, &response->code, sizeof(response->code));
  sdb_binary_reader_read(&reader, &response->points_count, sizeof(response->points_count));
  sdb_binary_reader_read_pointer(&reader, &response->points, sizeof(sdb_data_point_t) * response->points_count);

  if (!reader.success) {
    sdb_free(response);
    return NULL;
  }

  return response;
}

void sdb_read_response_serialize(sdb_read_response_t *response, sdb_socket_t socket) {
  sdb_socket_send(socket, &response->code, sizeof(response->code));
  sdb_socket_send(socket, &response->points_count, sizeof(response->points_count));
  sdb_socket_send(socket, response->points, sizeof(sdb_data_point_t) * response->points_count);
}

/* ========== truncate request ========== */

sdb_packet_t *sdb_truncate_request_create(sdb_data_series_id_t data_series_id) {
  sdb_packet_t *packet = (sdb_packet_t *)sdb_alloc(sizeof(sdb_packet_t));
  packet->header.type = SDB_TRUNCATE_REQUEST;
  packet->header.payload_size = sizeof(data_series_id);

  sdb_truncate_request_t *request = (sdb_truncate_request_t *)sdb_alloc(sizeof(sdb_truncate_request_t));
  request->data_series_id = data_series_id;

  packet->payload = request;
  return packet;
}

sdb_truncate_request_t *sdb_truncate_request_deserialize(void *payload, size_t size) {
  sdb_truncate_request_t *request = (sdb_truncate_request_t *)sdb_alloc(sizeof(sdb_truncate_request_t));

  sdb_binary_reader_t reader = {0};
  sdb_binary_reader_init(&reader, payload, size);

  sdb_binary_reader_read(&reader, &request->data_series_id, sizeof(request->data_series_id));

  if (!reader.success) {
    sdb_free(request);
    return NULL;
  }

  return request;
}

void sdb_truncate_request_serialize(sdb_truncate_request_t *request, sdb_socket_t socket) {
  sdb_socket_send(socket, &request->data_series_id, sizeof(request->data_series_id));
}

/* ========== generic methods ========== */

sdb_packet_t *sdb_packet_receive(sdb_socket_t socket) {
  sdb_packet_header_t header = {0};

  if (sdb_socket_receive(socket, &header, sizeof(sdb_packet_header_t)) < sizeof(sdb_packet_header_t)) {
    return NULL;
  }

  if (header.payload_size > SDB_PACKET_MAX_LEN) {
    return NULL;
  }

  sdb_packet_t *packet = (sdb_packet_t *)sdb_alloc(sizeof(sdb_packet_t));
  packet->header = header;
  packet->_raw_payload = sdb_alloc(header.payload_size);

  if (sdb_socket_receive(socket, packet->_raw_payload, header.payload_size) != header.payload_size) {
    sdb_packet_destroy(packet);
    return NULL;
  }

  switch (header.type) {
    case SDB_WRITE_REQUEST:packet->payload = sdb_write_request_deserialize(packet->_raw_payload, header.payload_size);
      break;
    case SDB_SIMPLE_RESPONSE:
      packet->payload =
          sdb_simple_response_deserialize(packet->_raw_payload, header.payload_size);
      break;
    case SDB_READ_REQUEST:packet->payload = sdb_read_request_deserialize(packet->_raw_payload, header.payload_size);
      break;
    case SDB_READ_RESPONSE:packet->payload = sdb_read_response_deserialize(packet->_raw_payload, header.payload_size);
      break;
    default: packet->payload = NULL;
  }

  if (packet->payload == NULL) {
    sdb_packet_destroy(packet);
    return NULL;
  }

  return packet;
}

int sdb_packet_send(sdb_packet_t *packet, sdb_socket_t socket) {
  sdb_socket_send(socket, &packet->header, sizeof(packet->header));

  switch (packet->header.type) {
    case SDB_WRITE_REQUEST:sdb_write_request_serialize((sdb_write_request_t *)packet->payload, socket);
      break;
    case SDB_SIMPLE_RESPONSE:sdb_simple_response_serialize((sdb_simple_response_t *)packet->payload, socket);
      break;
    case SDB_READ_REQUEST:sdb_read_request_serialize((sdb_read_request_t *)packet->payload, socket);
      break;
    case SDB_READ_RESPONSE:sdb_read_response_serialize((sdb_read_response_t *)packet->payload, socket);
      break;
    default: break;
  }

  // TODO: (pburzynski) return a value
  return 0;
}

int sdb_packet_send_and_destroy(sdb_packet_t *packet, sdb_socket_t socket) {
  int status = sdb_packet_send(packet, socket);
  sdb_packet_destroy(packet);

  return status;
}

void sdb_packet_destroy(sdb_packet_t *packet) {
  if (packet->payload != NULL) {
    sdb_free(packet->payload);
  }

  if (packet->_raw_payload != NULL) {
    sdb_free(packet->_raw_payload);
  }

  sdb_free(packet);
}