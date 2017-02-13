//
// Created by Pawel Burzynski on 22/01/2017.
//

#include <cstdio>
#include "rw-lock.h"

namespace shakadb {

RwLock::RwLock() {
  this->rwlock = PTHREAD_RWLOCK_INITIALIZER;
}

RwLock::~RwLock() {
  pthread_rwlock_unlock(&this->rwlock);
  pthread_rwlock_destroy(&this->rwlock);
}

std::shared_ptr<RwLockScope> RwLock::LockRead() {
  pthread_rwlock_rdlock(&this->rwlock);
  return std::make_shared<RwLockScope>(&this->rwlock);
}

std::shared_ptr<RwLockScope> RwLock::LockWrite() {
  pthread_rwlock_wrlock(&this->rwlock);
  return std::make_shared<RwLockScope>(&this->rwlock);
}

}