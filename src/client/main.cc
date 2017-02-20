//
// Created by Pawel Burzynski on 09/02/2017.
//

#include <src/utils/stopwatch.h>
#include <src/data-points-reader.h>
#include "session-manager.h"

int main() {
  shakadb::SessionManager manager;
  int session_id = manager.OpenSession("localhost", 8099);

  if (session_id == shakadb::SessionManager::kInvalidSession) {
    printf("Unable to connect\n");
    return -1;
  }

  if (!manager.GetSessionById(session_id)->Ping()) {
    printf("Ping failure\n");
    return -1;
  }

  shakadb::Session *session = manager.GetSessionById(session_id);
  shakadb::DataPointsReader *reader = session->ReadPoints("USD_AUD", A_MIN_TIMESTAMP, A_MAX_TIMESTAMP);
  shakadb::data_point_t points[1024] = {0};
  int remaining = reader->GetDataPointsCount();

  while (remaining > 0) {
    int read = reader->ReadDataPoints(points, 1024);
    remaining -= read;

    for (int i = 0; i < read; i++) {
      printf("%llu -> %f\n", points[i].time, points[i].value);
    }
  }

  delete reader;

//  shakadb::data_point_t points[100] = {0};
//  shakadb::Stopwatch sw;
//  shakadb::Session *session = manager.GetSessionById(session_id);
//
//  sw.Start();
//
//  for (int i = 0; i < 10; i++) {
//    for (int j = 0; j < 100; j++) {
//      points[j].time = i * 100 + j + 1;
//      points[j].value = j;
//    }
//
//    session->WritePoints("USD_AUD", points, 100);
//  }
//
//  sw.Stop();
//  printf("Elapsed: %f[s]\n", sw.GetElapsedSeconds());

  return 0;
}