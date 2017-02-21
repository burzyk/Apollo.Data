//
// Created by Pawel Burzynski on 09/02/2017.
//

#include <src/utils/stopwatch.h>
#include <src/data-points-reader.h>
#include "session.h"

int main() {
  shakadb::Session *session = shakadb::Session::Open("localhost", 8099);

  if (session == nullptr) {
    printf("Unable to connect\n");
    return -1;
  }

  if (!session->Ping()) {
    printf("Ping failure\n");
    return -1;
  }

  shakadb::DataPointsReader *reader = session->ReadPoints(
      "USD_AUD",
      shakadb::data_point_t::kMinTimestamp,
      shakadb::data_point_t::kMaxTimestamp);
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

  delete session;

  return 0;
}