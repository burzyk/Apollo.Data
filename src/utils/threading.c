//
// Created by Pawel Burzynski on 14/04/2017.
//

#include "src/utils/threading.h"
#include "src/utils/memory.h"
#include "src/c_common.h"

sdb_thread_t *sdb_thread_create() {
  return (sdb_thread_t *)sdb_alloc(sizeof(sdb_thread_t));
}

void sdb_thread_sleep(int milliseconds) {
  struct timespec ts = {
      .tv_sec = milliseconds / 1000, .tv_nsec = (milliseconds % 1000) * 1000000
  };
  nanosleep(&ts, NULL);
}

void sdb_thread_start(sdb_thread_t *thread, sdb_thread_routine_t routine, void *data) {
  if (!pthread_create(&thread->thread, NULL, routine, data)) {
    die("Unable to start a thread");
  }
}

void sdb_thread_join_and_destroy(sdb_thread_t *thread) {
  if (!pthread_join(thread->thread, NULL)) {
    die("Unable to join a thread");
  }

  sdb_free(thread);
}

sdb_rwlock_t *sdb_rwlock_create() {
  sdb_rwlock_t *lock = (sdb_rwlock_t *)sdb_alloc(sizeof(sdb_rwlock_t));

  if (!pthread_rwlock_init(&lock->lock, NULL)) {
    die("Unable to create a rwlock");
  }

  return lock;
}

void sdb_rwlock_rdlock(sdb_rwlock_t *lock) {
  if (!pthread_rwlock_rdlock(&lock->lock)) {
    die("Unable to lock for reading");
  }
}

void sdb_rwlock_wrlock(sdb_rwlock_t *lock) {
  if (!pthread_rwlock_wrlock(&lock->lock)) {
    die("Unable to lock for writing");
  }
}

void sdb_rwlock_upgrade(sdb_rwlock_t *lock) {
  sdb_rwlock_unlock(lock);
  sdb_rwlock_wrlock(lock);
}

void sdb_rwlock_unlock(sdb_rwlock_t *lock) {
  pthread_rwlock_unlock(&lock->lock);
}

void sdb_rwlock_destroy(sdb_rwlock_t *lock) {
  pthread_rwlock_unlock(&lock->lock);
  pthread_rwlock_destroy(&lock->lock);
}