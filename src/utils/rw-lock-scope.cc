//
// Created by Pawel Burzynski on 22/01/2017.
//

#include "rw-lock-scope.h"

namespace apollo {

RwLockScope::RwLockScope(pthread_rwlock_t *lock, pthread_mutex_t *upgrade_lock) {
  this->lock = lock;
  this->upgrade_lock = upgrade_lock;
}

RwLockScope::~RwLockScope() {
  pthread_rwlock_unlock(this->lock);
}

void RwLockScope::UpgradeToWrite() {
  pthread_mutex_lock(this->upgrade_lock);
  pthread_rwlock_unlock(this->lock);
  pthread_rwlock_wrlock(this->lock);
  pthread_mutex_unlock(this->upgrade_lock);
}

}