//
// Created by Pawel Burzynski on 09/02/2017.
//

#include <src/utils/stopwatch.h>
#include <src/storage/data-points-reader.h>
#include "session.h"

void read_points(shakadb::Session *session) {
  auto iterator = std::unique_ptr<shakadb::ReadPointsIterator>(session->ReadPoints(
      "USD_AUD",
      shakadb::data_point_t::kMinTimestamp,
      shakadb::data_point_t::kMaxTimestamp));
  shakadb::data_point_t prev = {0};
  shakadb::data_point_t curr = {0};
  int total_read = 0;
  shakadb::Stopwatch sw;

  sw.Start();

  while (iterator->MoveNext()) {
    for (int i = 0; i < iterator->CurrentDataPointsCount(); i++) {
      curr = iterator->CurrentDataPoints()[i];
      if (prev.time >= curr.time) {
        printf("Error reading prev: %llu, curr: %llu\n", prev.time, curr.time);
        return;
      }

//      printf("%llu -> %f\n", curr.time, curr.value);
      prev = curr;
      total_read++;
    }
  }

  sw.Stop();

  printf("Total read points: %d in %f[s]\n", total_read, sw.GetElapsedSeconds());
}

void write_points(shakadb::Session *session) {
  int points_count = 1000;

  shakadb::data_point_t points[points_count];
  shakadb::Stopwatch sw;
  shakadb::timestamp_t time = 1;

  sw.Start();

  for (int i = 0; i < 100000; i++) {
    for (int j = 0; j < points_count; j++) {
      points[j].time = time++;
      points[j].value = j;
    }

    session->WritePoints("USD_AUD", points, points_count);
  }

  sw.Stop();
  printf("Elapsed: %f[s]\n", sw.GetElapsedSeconds());
}

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

  read_points(session);
//  write_points(session);

  delete session;

  return 0;
}