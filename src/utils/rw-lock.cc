//
// Created by Pawel Burzynski on 22/01/2017.
//

#include "rw-lock.h"

namespace apollo {

RwLock::RwLock() {
  this->rwlock = PTHREAD_RWLOCK_INITIALIZER;
}

RwLock::~RwLock() {
  this->Unlock();
  pthread_rwlock_destroy(&this->rwlock);
}

void RwLock::LockWrite() {
  pthread_rwlock_wrlock(&this->rwlock);
}

void RwLock::LockRead() {
  pthread_rwlock_rdlock(&this->rwlock);
}

void RwLock::Unlock() {
  pthread_rwlock_unlock(&this->rwlock);
}

}