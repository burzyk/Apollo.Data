//
// Created by Pawel Burzynski on 14/02/2017.
//

#include "monitor-scope.h"

namespace shakadb {

MonitorScope::MonitorScope(pthread_mutex_t *mutex, pthread_cond_t *cond) {
  this->mutex = mutex;
  this->cond = cond;
}

MonitorScope::~MonitorScope() {
  this->Exit();
}

void MonitorScope::Reenter() {
  pthread_mutex_lock(this->mutex);
}

void MonitorScope::Signal() {
  pthread_cond_signal(this->cond);
}

void MonitorScope::Wait() {
  pthread_cond_wait(this->cond, this->mutex);
}

void MonitorScope::Exit() {
  pthread_mutex_unlock(this->mutex);
}

}