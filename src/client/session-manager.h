//
// Created by Pawel Burzynski on 08/02/2017.
//

#ifndef SHAKADB_STORAGE_SESSIONMANAGER_H
#define SHAKADB_STORAGE_SESSIONMANAGER_H

#include <vector>
#include <map>
#include "session.h"

namespace shakadb {

class SessionManager {
 public:
  static const int kInvalidSession = -1;

  SessionManager();
  ~SessionManager();

  int OpenSession(std::string server, int port);
  Session *GetSessionById(int session_id);
  void CloseSession(int session_id);
 private:
  std::map<int, Session *> active_sessions;
  int current_session_id;
};

}

#endif //SHAKADB_STORAGE_SESSIONMANAGER_H
