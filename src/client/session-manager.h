//
// Created by Pawel Burzynski on 08/02/2017.
//

#ifndef APOLLO_STORAGE_SESSIONMANAGER_H
#define APOLLO_STORAGE_SESSIONMANAGER_H

#include <vector>
#include <map>
#include "session.h"

namespace apollo {

class SessionManager {
 public:
  SessionManager();
  Session *OpenSession();
  Session *GetSessionById(int session_id);
  void CloseSession(int session_id);
 private:
  std::map<int, Session *> active_sessions;
  int current_session_id;
};

}

#endif //APOLLO_STORAGE_SESSIONMANAGER_H
