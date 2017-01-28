//
// Created by Pawel Burzynski on 22/01/2017.
//

#ifndef APOLLO_STORAGE_RWLOCKSCOPE_H
#define APOLLO_STORAGE_RWLOCKSCOPE_H

#include <pthread.h>

namespace apollo {

class RwLockScope {
 public:
  RwLockScope(pthread_rwlock_t *lock, pthread_mutex_t *upgrade_lock);
  ~RwLockScope();

  void UpgradeToWrite();
 private:
  pthread_rwlock_t *lock;
  pthread_mutex_t *upgrade_lock;
};

}

#endif //APOLLO_STORAGE_RWLOCKSCOPE_H
