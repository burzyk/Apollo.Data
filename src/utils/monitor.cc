//
// Created by Pawel Burzynski on 14/02/2017.
//

#include "monitor.h"

namespace shakadb {

Monitor::Monitor() {
  this->mutex = PTHREAD_MUTEX_INITIALIZER;
  this->cond = PTHREAD_COND_INITIALIZER;
}

Monitor::~Monitor() {
  pthread_mutex_unlock(&this->mutex);

  pthread_cond_destroy(&this->cond);
  pthread_mutex_destroy(&this->mutex);
}

std::shared_ptr<MonitorScope> Monitor::Enter() {
  pthread_mutex_lock(&this->mutex);
  return std::make_shared<MonitorScope>(&this->mutex, &this->cond);
}

}