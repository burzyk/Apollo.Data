//
// Created by Pawel Burzynski on 22/01/2017.
//

#include "rw-lock.h"

namespace apollo {

RwLock::RwLock() {
  this->rwlock = PTHREAD_RWLOCK_INITIALIZER;
}

RwLock::~RwLock() {
  pthread_rwlock_unlock(&this->rwlock);
  pthread_rwlock_destroy(&this->rwlock);
}

RwLockScope *RwLock::LockWrite() {
  pthread_rwlock_wrlock(&this->rwlock);
  return new RwLockScope(&this->rwlock);
}

RwLockScope *RwLock::LockRead() {
  pthread_rwlock_rdlock(&this->rwlock);
  return new RwLockScope(&this->rwlock);
}

}