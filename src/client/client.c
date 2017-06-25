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
// Created by Pawel Burzynski on 26/02/2017.
//

#include "src/client/client.h"
#include "src/client/session.h"
#include "client.h"

int shakadb_session_open(shakadb_session_t *session, const char *server, int port) {
  session->_session = sdb_client_session_create(server, port);
  session->_read_opened = 0;
  return session->_session == NULL ? SHAKADB_RESULT_CONNECT_ERROR : SHAKADB_RESULT_OK;
}

void shakadb_session_close(shakadb_session_t *session) {
  sdb_client_session_t *s = (sdb_client_session_t *)session->_session;

  if (s == NULL) {
    return;
  }

  sdb_client_session_destroy(s);
  session->_session = NULL;
}

int shakadb_write_points(shakadb_session_t *session,
                         shakadb_data_series_id_t series_id,
                         shakadb_data_point_t *points,
                         int points_count) {
  sdb_client_session_t *s = (sdb_client_session_t *)session->_session;
  return !sdb_client_session_write_points(s, series_id, (sdb_data_point_t *)points, points_count)
         ? SHAKADB_RESULT_OK
         : SHAKADB_RESULT_GENERIC_ERROR;
}

int shakadb_truncate_data_series(shakadb_session_t *session, shakadb_data_series_id_t series_id) {
  sdb_client_session_t *s = (sdb_client_session_t *)session->_session;
  return !sdb_client_session_truncate_data_series(s, series_id)
         ? SHAKADB_RESULT_OK
         : SHAKADB_RESULT_GENERIC_ERROR;
}

int shakadb_read_points(shakadb_session_t *session,
                        shakadb_data_series_id_t series_id,
                        shakadb_timestamp_t begin,
                        shakadb_timestamp_t end,
                        int points_per_packet,
                        shakadb_data_points_iterator_t *iterator) {
  iterator->_iterator = NULL;
  iterator->_session = NULL;
  iterator->points = NULL;
  iterator->points_count = 0;

  if (session->_read_opened) {
    return SHAKADB_RESULT_MULTIPLE_READS_ERROR;
  }

  sdb_client_session_t *s = (sdb_client_session_t *)session->_session;
  iterator->_iterator = sdb_client_session_read_points(s, series_id, begin, end, points_per_packet);
  iterator->points = NULL;
  iterator->points_count = -1;
  iterator->_session = session;
  int result = iterator->_iterator == NULL ? SHAKADB_RESULT_GENERIC_ERROR : SHAKADB_RESULT_OK;

  if (result == SHAKADB_RESULT_OK) {
    session->_read_opened = 1;
  }

  return result;
}

int shakadb_read_latest_point(shakadb_session_t *session,
                              shakadb_data_series_id_t series_id,
                              shakadb_data_point_t *latest) {
  if (session->_read_opened) {
    return SHAKADB_RESULT_MULTIPLE_READS_ERROR;
  }

  sdb_data_point_t result = {.value=0, .time=0};

  if (sdb_client_session_read_latest_point(session->_session, series_id, &result)) {
    return SHAKADB_RESULT_GENERIC_ERROR;
  }

  latest->time = result.time;
  latest->value = result.value;

  return SHAKADB_RESULT_OK;
}

int shakadb_data_points_iterator_next(shakadb_data_points_iterator_t *iterator) {
  sdb_data_points_iterator_t *i = (sdb_data_points_iterator_t *)iterator->_iterator;

  if (iterator->_iterator == NULL) {
    return 0;
  }

  if (sdb_data_points_iterator_next(i)) {
    iterator->points = (shakadb_data_point_t *)i->points;
    iterator->points_count = i->points_count;
    return 1;
  } else {
    sdb_data_points_iterator_destroy(i);
    iterator->_iterator = NULL;
    iterator->points = NULL;
    iterator->points_count = -1;
    iterator->_session->_read_opened = 0;
    return 0;
  }
}
