//
// Created by Pawel Burzynski on 22/01/2017.
//

#ifndef APOLLO_STORAGE_RW_LOCK_H
#define APOLLO_STORAGE_RW_LOCK_H

#include <pthread.h>

namespace apollo {

class RwLock {
 public:
  RwLock();
  ~RwLock();

  void LockWrite();
  void LockRead();
  void Unlock();
 private:
  pthread_rwlock_t rwlock;
};

}

#endif //APOLLO_STORAGE_RWLOCK_H
