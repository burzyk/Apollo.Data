//
// Created by Pawel Burzynski on 22/01/2017.
//

#ifndef SHAKADB_STORAGE_RWLOCKSCOPE_H
#define SHAKADB_STORAGE_RWLOCKSCOPE_H

#include <pthread.h>

namespace shakadb {

class RwLockScope {
 public:
  RwLockScope(pthread_rwlock_t *lock);
  ~RwLockScope();

  void UpgradeToWrite();
 private:
  pthread_rwlock_t *lock;
};

}

#endif //SHAKADB_STORAGE_RWLOCKSCOPE_H
