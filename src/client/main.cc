//
// Created by Pawel Burzynski on 09/02/2017.
//

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
  }

  shakadb::data_point_t points[1024] = {0};

  for (int i = 0; i < 1000; i++) {
    for (int j = 0; j < 1024; j++) {
      points[j].time = i * 1024 + j;
      points[j].value = j;
    }

    manager.GetSessionById(session_id)->WritePoints("USD_AUD", points, 1024);
  }

  return 0;
}