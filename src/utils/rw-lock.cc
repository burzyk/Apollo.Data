//
// Created by Pawel Burzynski on 22/01/2017.
//

#include "rw-lock.h"

namespace apollo {

RwLock::RwLock() {
  this->rwlock = PTHREAD_RWLOCK_INITIALIZER;
  this->upgrade_lock = PTHREAD_MUTEX_INITIALIZER;
}

RwLock::~RwLock() {
  pthread_mutex_unlock(&this->upgrade_lock);
  pthread_rwlock_unlock(&this->rwlock);

  pthread_mutex_destroy(&this->upgrade_lock);
  pthread_rwlock_destroy(&this->rwlock);
}

RwLockScope *RwLock::LockRead() {
  pthread_rwlock_rdlock(&this->rwlock);
  return new RwLockScope(&this->rwlock, &this->upgrade_lock);
}

}