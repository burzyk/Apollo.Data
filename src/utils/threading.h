//
// Created by Pawel Burzynski on 14/04/2017.
//

#ifndef SRC_UTILS_THREADING_H_
#define SRC_UTILS_THREADING_H_

#include <pthread.h>

typedef void *(*sdb_thread_routine_t)(void *);

typedef struct sdb_thread_s {
  pthread_t thread;
} sdb_thread_t;

typedef struct sdb_rwlock_s {
  pthread_rwlock_t lock;
} sdb_rwlock_t;

typedef struct sdb_mutex_s {
  pthread_mutex_t mutex;
} sdb_mutex_t;

sdb_thread_t *sdb_thread_start(sdb_thread_routine_t routine, void *data);
void sdb_thread_join_and_destroy(sdb_thread_t *thread);

sdb_rwlock_t *sdb_rwlock_create();
void sdb_rwlock_rdlock(sdb_rwlock_t *lock);
void sdb_rwlock_wrlock(sdb_rwlock_t *lock);
void sdb_rwlock_upgrade(sdb_rwlock_t *lock);
void sdb_rwlock_unlock(sdb_rwlock_t *lock);
void sdb_rwlock_destroy(sdb_rwlock_t *lock);

sdb_mutex_t *sdb_mutex_create();
void sdb_mutex_lock(sdb_mutex_t *monitor);
void sdb_mutex_unlock(sdb_mutex_t *monitor);
void sdb_mutex_destroy(sdb_mutex_t *monitor);

#endif  // SRC_UTILS_THREADING_H_
