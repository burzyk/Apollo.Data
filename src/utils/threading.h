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

#ifndef SRC_UTILS_THREADING_H_
#define SRC_UTILS_THREADING_H_

#include <pthread.h>

typedef void *(*sdb_thread_routine_t)(void *);

typedef struct sdb_thread_s {
  pthread_t _thread;
} sdb_thread_t;

typedef struct sdb_rwlock_s {
  pthread_rwlock_t _lock;
} sdb_rwlock_t;

typedef struct sdb_mutex_s {
  pthread_mutex_t _mutex;
} sdb_mutex_t;

sdb_thread_t *sdb_thread_start(sdb_thread_routine_t routine, void *data);
void sdb_thread_join_and_destroy(sdb_thread_t *thread);
int sdb_thread_get_current_id();
void sdb_thread_sleep(int milliseconds);

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
