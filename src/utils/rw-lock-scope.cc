//
// Created by Pawel Burzynski on 22/01/2017.
//

#include "rw-lock-scope.h"

namespace apollo {

RwLockScope::RwLockScope(pthread_rwlock_t *lock) {
  this->lock = lock;
}

RwLockScope::~RwLockScope() {
  pthread_rwlock_unlock(this->lock);
}

}