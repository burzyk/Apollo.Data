//
// Created by Pawel Burzynski on 26/02/2017.
//

#ifndef SHAKADB_CLIENT_H_H
#define SHAKADB_CLIENT_H_H

#include <cstdint>

extern "C" {

typedef uint64_t shakadb_timestamp_t;
typedef uint32_t shakadb_result_t;

#define SHAKADB_MAX_TIMESTAMP UINT64_MAX
#define SHAKADB_MIN_TIMESTAMP 0

#define SHAKADB_RESULT_OK ((shakadb_result_t)0)
#define SHAKADB_RESULT_ERROR  ((shakadb_result_t)-1)

typedef struct __attribute__((packed)) {
  shakadb_timestamp_t time;
  float value;
} shakadb_data_point_t;

typedef struct {
  void *_session;
} shakadb_session_t;

typedef struct {
  shakadb_data_point_t *points;
  int points_count;
  void *_iterator;
} shakadb_read_points_iterator_t;

shakadb_result_t shakadb_open_session(shakadb_session_t *session, const char *server, int port);
shakadb_result_t shakadb_destroy_session(shakadb_session_t *session);
shakadb_result_t shakadb_ping(shakadb_session_t *session);
shakadb_result_t shakadb_write_points(shakadb_session_t *session,
                                      const char *series_name,
                                      shakadb_data_point_t *points,
                                      int points_count);
shakadb_result_t shakadb_read_points(shakadb_session_t *session,
                                     const char *series_name,
                                     shakadb_timestamp_t begin,
                                     shakadb_timestamp_t end,
                                     shakadb_read_points_iterator_t *iterator);
int shakadb_read_points_iterator_next(shakadb_read_points_iterator_t *iterator);

};

#endif //SHAKADB_CLIENT_H_H
