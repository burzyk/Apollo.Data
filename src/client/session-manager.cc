//
// Created by Pawel Burzynski on 08/02/2017.
//

#include "session-manager.h"

namespace apollo {

SessionManager::SessionManager() {
  this->current_session_id = 0;
}

Session *SessionManager::OpenSession() {
  Session *session = new Session();
  return this->active_sessions[this->current_session_id++] = session;
}

Session *SessionManager::GetSessionById(int session_id) {
  return this->active_sessions.find(session_id) == this->active_sessions.end()
         ? nullptr
         : this->active_sessions[session_id];
}

void SessionManager::CloseSession(int session_id) {
  Session *session = this->GetSessionById(session_id);

  if (session == nullptr) {
    return;
  }

  this->active_sessions.erase(session_id);
  delete session;
}

}