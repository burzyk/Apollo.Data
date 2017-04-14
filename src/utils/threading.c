//
// Created by Pawel Burzynski on 14/04/2017.
//

#include "src/utils/threading.h"
#include "src/utils/memory.h"

sdb_thread_t *sdb_thread_create() {
  return (sdb_thread_t *)sdb_alloc(sizeof(sdb_thread_t));
}

void sdb_thread_sleep(int milliseconds) {
  struct timespec ts = {
      .tv_sec = milliseconds / 1000, .tv_nsec = (milliseconds % 1000) * 1000000
  };
  nanosleep(&ts, NULL);
}

int sdb_thread_start(sdb_thread_t *thread, sdb_thread_routine_t routine, void *data) {
  return pthread_create(&thread->thread, NULL, routine, data);
}

int sdb_thread_join_and_destroy(sdb_thread_t *thread) {
  int status = pthread_join(thread->thread, NULL);
  sdb_free(thread);

  return status;
}