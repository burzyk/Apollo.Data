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

#ifndef SRC_CLIENT_SESSION_H_
#define SRC_CLIENT_SESSION_H_

#include "src/protocol.h"
#include "src/client/data-points-iterator.h"

typedef struct sdb_client_session_s {
  sdb_socket_t _sock;
} sdb_client_session_t;

sdb_client_session_t *sdb_client_session_create(const char *server, int port);
void sdb_client_session_destroy(sdb_client_session_t *session);
int sdb_client_session_write_points(sdb_client_session_t *session,
                                    sdb_data_series_id_t series_id,
                                    sdb_data_point_t *points,
                                    int count);
int sdb_client_session_truncate_data_series(sdb_client_session_t *session, sdb_data_series_id_t series_id);
sdb_data_points_iterator_t *sdb_client_session_read_points(sdb_client_session_t *session,
                                                           sdb_data_series_id_t series_id,
                                                           sdb_timestamp_t begin,
                                                           sdb_timestamp_t end,
                                                           int points_per_packet);

#endif  // SRC_CLIENT_SESSION_H_
