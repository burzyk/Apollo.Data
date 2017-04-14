//
// Created by Pawel Burzynski on 14/04/2017.
//

#ifndef SRC_UTILS_THREADING_H_
#define SRC_UTILS_THREADING_H_

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*sdb_thread_routine_t)(void *);

typedef struct sdb_thread_s {
  pthread_t thread;
} sdb_thread_t;

typedef struct sdb_rwlock_s {
  pthread_rwlock_t lock;
} sdb_rwlock_t;

typedef struct sdb_monitor_s {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} sdb_monitor_t;

sdb_thread_t *sdb_thread_create();
void sdb_thread_sleep(int milliseconds);
void sdb_thread_start(sdb_thread_t *thread, sdb_thread_routine_t routine, void *data);
void sdb_thread_join_and_destroy(sdb_thread_t *thread);

sdb_rwlock_t *sdb_rwlock_create();
void sdb_rwlock_rdlock(sdb_rwlock_t *lock);
void sdb_rwlock_wrlock(sdb_rwlock_t *lock);
void sdb_rwlock_upgrade(sdb_rwlock_t *lock);
void sdb_rwlock_unlock(sdb_rwlock_t *lock);
void sdb_rwlock_destroy(sdb_rwlock_t *lock);

sdb_monitor_t *sdb_monitor_create();
void sdb_monitor_enter(sdb_monitor_t *monitor);
void sdb_monitor_signal(sdb_monitor_t *monitor);
void sdb_monitor_wait(sdb_monitor_t *monitor);
void sdb_monitor_exit(sdb_monitor_t *monitor);
void sdb_monitor_destroy(sdb_monitor_t *monitor);

#ifdef __cplusplus
}
#endif

#endif  // SRC_UTILS_THREADING_H_
