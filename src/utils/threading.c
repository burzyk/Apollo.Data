/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//
// Created by Pawel Burzynski on 14/04/2017.
//

#include "src/utils/threading.h"
#include "src/utils/memory.h"
#include "src/common.h"

sdb_thread_t *sdb_thread_start(sdb_thread_routine_t routine, void *data) {
  sdb_thread_t *thread = (sdb_thread_t *)sdb_alloc(sizeof(sdb_thread_t));
  if (pthread_create(&thread->_thread, NULL, routine, data)) {
    die("Unable to start a thread");
  }

  return thread;
}

void sdb_thread_join_and_destroy(sdb_thread_t *thread) {
  if (pthread_join(thread->_thread, NULL)) {
    die("Unable to join a thread");
  }

  sdb_free(thread);
}

sdb_rwlock_t *sdb_rwlock_create() {
  sdb_rwlock_t *lock = (sdb_rwlock_t *)sdb_alloc(sizeof(sdb_rwlock_t));

  if (pthread_rwlock_init(&lock->_lock, NULL)) {
    die("Unable to create a rwlock");
  }

  return lock;
}

void sdb_rwlock_rdlock(sdb_rwlock_t *lock) {
  if (pthread_rwlock_rdlock(&lock->_lock)) {
    die("Unable to lock for reading");
  }
}

void sdb_rwlock_wrlock(sdb_rwlock_t *lock) {
  if (pthread_rwlock_wrlock(&lock->_lock)) {
    die("Unable to lock for writing");
  }
}

void sdb_rwlock_upgrade(sdb_rwlock_t *lock) {
  sdb_rwlock_unlock(lock);
  sdb_rwlock_wrlock(lock);
}

void sdb_rwlock_unlock(sdb_rwlock_t *lock) {
  if (pthread_rwlock_unlock(&lock->_lock)) {
    die("Unable to unlock the lock");
  }
}

void sdb_rwlock_destroy(sdb_rwlock_t *lock) {
  pthread_rwlock_unlock(&lock->_lock);
  pthread_rwlock_destroy(&lock->_lock);

  sdb_free(lock);
}

sdb_mutex_t *sdb_mutex_create() {
  sdb_mutex_t *monitor = (sdb_mutex_t *)sdb_alloc(sizeof(sdb_mutex_t));

  if (pthread_mutex_init(&monitor->_mutex, NULL)) {
    die("Unable to create a mutex");
  }

  return monitor;
}

void sdb_mutex_lock(sdb_mutex_t *monitor) {
  if (pthread_mutex_lock(&monitor->_mutex)) {
    die("Unable to enter server_lock");
  }
}

void sdb_mutex_unlock(sdb_mutex_t *monitor) {
  if (pthread_mutex_unlock(&monitor->_mutex)) {
    die("Unable to exit server_lock");
  }
}

void sdb_mutex_destroy(sdb_mutex_t *monitor) {
  pthread_mutex_unlock(&monitor->_mutex);
  pthread_mutex_destroy(&monitor->_mutex);

  sdb_free(monitor);
}
