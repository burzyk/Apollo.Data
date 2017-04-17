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
// Created by Pawel Burzynski on 08/02/2017.
//

#include "src/client/session.h"
#include "src/utils/memory.h"

int sdb_client_session_send_with_simple_response(sdb_client_session_t *session, sdb_packet_t *request);

sdb_client_session_t *sdb_client_session_create(const char *server, int port) {
  sdb_socket_t sock = sdb_socket_connect(server, port);

  if (sock == SDB_INVALID_SOCKET) {
    return NULL;
  }

  sdb_client_session_t *session = (sdb_client_session_t *)sdb_alloc(sizeof(sdb_client_session_t));
  session->_sock = sock;

  return session;
}

void sdb_client_session_destroy(sdb_client_session_t *session) {
  sdb_socket_close(session->_sock);
  sdb_free(session);
}

int sdb_client_session_write_points(sdb_client_session_t *session,
                                    sdb_data_series_id_t series_id,
                                    sdb_data_point_t *points,
                                    int count) {
  sdb_packet_t *request = sdb_write_request_create(series_id, points, count);
  return sdb_client_session_send_with_simple_response(session, request);
}

int sdb_client_session_truncate_data_series(sdb_client_session_t *session, sdb_data_series_id_t series_id) {
  sdb_packet_t *request = sdb_truncate_request_create(series_id);
  return sdb_client_session_send_with_simple_response(session, request);
}

sdb_data_points_iterator_t *sdb_client_session_read_points(sdb_client_session_t *session,
                                                           sdb_data_series_id_t series_id,
                                                           sdb_timestamp_t begin,
                                                           sdb_timestamp_t end) {
  if (sdb_packet_send_and_destroy(sdb_read_request_create(series_id, begin, end), session->_sock)) {
    return NULL;
  }

  return sdb_data_points_iterator_create(session->_sock);
}

int sdb_client_session_send_with_simple_response(sdb_client_session_t *session, sdb_packet_t *request) {
  int send_status = sdb_packet_send_and_destroy(request, session->_sock);

  if (send_status) {
    return send_status;
  }

  sdb_packet_t *packet = sdb_packet_receive(session->_sock);

  if (packet == NULL) {
    return -1;
  }

  if (packet->header.type != SDB_SIMPLE_RESPONSE) {
    sdb_packet_destroy(packet);
    return -1;
  }

  sdb_simple_response_t *response = (sdb_simple_response_t *)packet->payload;
  int result = response->code != SDB_RESPONSE_OK;
  sdb_packet_destroy(packet);

  return result;
}
