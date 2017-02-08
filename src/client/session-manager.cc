//
// Created by Pawel Burzynski on 08/02/2017.
//

#include "session-manager.h"

namespace apollo {

SessionManager::SessionManager() {
  this->current_session_id = 0;
}

SessionManager::~SessionManager() {
  for (auto session: this->active_sessions) {
    delete session.second;
  }

  this->active_sessions.clear();
}

int SessionManager::OpenSession(std::string server, int port) {
  Session *session = Session::Open(server, port);
  int session_id = this->current_session_id++;

  if (session != nullptr) {
    this->active_sessions[session_id] = session;
  }

  return session == nullptr ? kInvalidSession : session_id;
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