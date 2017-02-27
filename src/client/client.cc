//
// Created by Pawel Burzynski on 26/02/2017.
//

#include <string>
#include "client.h"
#include "session.h"

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

shakadb_result_t shakadb_ping(shakadb_session_t *session) {
  shakadb::Session *s = (shakadb::Session *)session->_session;
  return s->Ping() ? SHAKADB_RESULT_OK : SHAKADB_RESULT_ERROR;
}

shakadb_result_t shakadb_write_points(shakadb_session_t *session,
                                      const char *series_name,
                                      shakadb_data_point_t *points,
                                      int points_count) {
  shakadb::Session *s = (shakadb::Session *)session->_session;
  bool result = s->WritePoints(std::string(series_name), (shakadb::data_point_t *)points, points_count);
  return result ? SHAKADB_RESULT_OK : SHAKADB_RESULT_ERROR;
}

shakadb_result_t shakadb_read_points(shakadb_session_t *session,
                                     const char *series_name,
                                     shakadb_timestamp_t begin,
                                     shakadb_timestamp_t end,
                                     shakadb_read_points_iterator_t *iterator) {
  shakadb::Session *s = (shakadb::Session *)session->_session;
  iterator->_iterator = s->ReadPoints(std::string(series_name), begin, end);
  iterator->points = nullptr;
  iterator->points_count = -1;
  return iterator->_iterator == nullptr ? SHAKADB_RESULT_ERROR : SHAKADB_RESULT_OK;
}

int shakadb_read_points_iterator_next(shakadb_read_points_iterator_t *iterator) {
  shakadb::ReadPointsIterator *i = (shakadb::ReadPointsIterator *)iterator->_iterator;

  if (!i->MoveNext()) {
    delete i;
    iterator->_iterator = nullptr;
    iterator->points = nullptr;
    iterator->points_count = -1;
    return 0;
  } else {
    iterator->points = (shakadb_data_point_t *)i->CurrentDataPoints();
    iterator->points_count = i->CurrentDataPointsCount();
    return 1;
  }
}