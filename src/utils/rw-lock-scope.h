//
// Created by Pawel Burzynski on 22/01/2017.
//

#ifndef APOLLO_STORAGE_RWLOCKSCOPE_H
#define APOLLO_STORAGE_RWLOCKSCOPE_H

#include <pthread.h>

namespace apollo {

class RwLockScope {
 public:
  RwLockScope(pthread_rwlock_t *lock);
  ~RwLockScope();

 private:
  pthread_rwlock_t *lock;
};

}

#endif //APOLLO_STORAGE_RWLOCKSCOPE_H
