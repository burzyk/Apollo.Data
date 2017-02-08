//
// Created by Pawel Burzynski on 09/02/2017.
//

#include "session-manager.h"
int main() {
  apollo::SessionManager manager;
  int session_id = manager.OpenSession("localhost", 8099);

  if (session_id == apollo::SessionManager::kInvalidSession) {
    printf("Unable to connect\n");
    return -1;
  }

  manager.GetSessionById(session_id)->Ping();

  return 0;
}