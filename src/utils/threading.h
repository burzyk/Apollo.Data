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

sdb_thread_t *sdb_thread_create();
void sdb_thread_sleep(int milliseconds);
int sdb_thread_start(sdb_thread_t *thread, sdb_thread_routine_t routine, void *data);
int sdb_thread_join_and_destroy(sdb_thread_t *thread);



#ifdef __cplusplus
}
#endif

#endif  // SRC_UTILS_THREADING_H_
