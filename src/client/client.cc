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

#include <string>

#include "src/client/session.h"

extern "C" {

shakadb_result_t shakadb_open_session(shakadb_session_t *session, const char *server, int port) {
  session->_session = shakadb::Session::Open(std::string(server), port);
  return session->_session == nullptr ? SHAKADB_RESULT_ERROR : SHAKADB_RESULT_OK;
}

shakadb_result_t shakadb_destroy_session(shakadb_session_t *session) {
  shakadb::Session *s = (shakadb::Session *)session->_session;
  delete s;
  session->_session = nullptr;
  return SHAKADB_RESULT_OK;
}

shakadb_result_t shakadb_write_points(shakadb_session_t *session,
                                      data_series_id_t series_id,
                                      shakadb_data_point_t *points,
                                      int points_count) {
  shakadb::Session *s = (shakadb::Session *)session->_session;
  bool result = s->WritePoints(series_id, (sdb_data_point_t *)points, points_count);
  return result ? SHAKADB_RESULT_OK : SHAKADB_RESULT_ERROR;
}

shakadb_result_t shakadb_read_points(shakadb_session_t *session,
                                     data_series_id_t series_id,
                                     shakadb_timestamp_t begin,
                                     shakadb_timestamp_t end,
                                     shakadb_read_points_iterator_t *iterator) {
  shakadb::Session *s = (shakadb::Session *)session->_session;
  iterator->_iterator = s->ReadPoints(series_id, begin, end);
  iterator->points = nullptr;
  iterator->points_count = -1;
  return iterator->_iterator == nullptr ? SHAKADB_RESULT_ERROR : SHAKADB_RESULT_OK;
}

int shakadb_read_points_iterator_next(shakadb_read_points_iterator_t *iterator) {
  sdb_data_points_iterator_t *i = (sdb_data_points_iterator_t *)iterator->_iterator;

  if (iterator->_iterator == nullptr) {
    return 0;
  }

  if (sdb_data_points_iterator_next(i)) {
    iterator->_iterator = nullptr;
    iterator->points = nullptr;
    iterator->points_count = -1;
    return 0;
  } else {
    iterator->points = (shakadb_data_point_t *)i->points;
    iterator->points_count = i->points_count;
    return 1;
  }
}

}
