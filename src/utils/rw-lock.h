//
// Created by Pawel Burzynski on 22/01/2017.
//

#ifndef SHAKADB_STORAGE_RW_LOCK_H
#define SHAKADB_STORAGE_RW_LOCK_H

#include <pthread.h>
#include <memory>
#include "rw-lock-scope.h"

namespace shakadb {

class RwLock {
 public:
  RwLock();
  ~RwLock();

  std::shared_ptr<RwLockScope> LockRead();
  std::shared_ptr<RwLockScope> LockWrite();
 private:
  pthread_rwlock_t rwlock;
};

}

#endif //SHAKADB_STORAGE_RWLOCK_H
