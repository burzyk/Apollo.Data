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

#ifndef SRC_CTL_SESSION_H_
#define SRC_CTL_SESSION_H_

#include "src/network/protocol.h"

typedef struct session_s {
  int socket;
  read_response_t *read_response;
  int read_open;
} session_t;

session_t *session_create(const char *server, int port);
void session_destroy(session_t *session);
int session_write(session_t *session, series_id_t series_id, points_list_t *points);
int session_truncate(session_t *session, series_id_t series_id);
int session_read(session_t *session,
                 series_id_t series_id,
                 timestamp_t begin,
                 timestamp_t end,
                 uint64_t points_per_packet);
int session_read_next(session_t *session);
int session_read_latest(session_t *session, series_id_t series_id, data_point_t *latest);

#endif  // SRC_NETWORK_CTL_SESSION_H_
