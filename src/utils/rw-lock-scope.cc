//
// Created by Pawel Burzynski on 22/01/2017.
//

#include "rw-lock-scope.h"

namespace shakadb {

RwLockScope::RwLockScope(pthread_rwlock_t *lock) {
  this->lock = lock;
}

RwLockScope::~RwLockScope() {
  pthread_rwlock_unlock(this->lock);
}

void RwLockScope::UpgradeToWrite() {
  pthread_rwlock_unlock(this->lock);
  pthread_rwlock_wrlock(this->lock);
}

}