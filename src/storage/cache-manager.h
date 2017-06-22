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
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef SRC_STORAGE_CACHE_MANAGER_H_
#define SRC_STORAGE_CACHE_MANAGER_H_

#include <stdint.h>

#include "src/utils/threading.h"

typedef struct sdb_cache_consumer_info_s {
} sdb_cache_consumer_info_t;

typedef struct sdb_cache_consumer_s {
  void *consumer;
  sdb_cache_consumer_info_t *info;
} sdb_cache_consumer_t;

typedef struct sdb_cache_manager_s {
  uint64_t _soft_limit;
  uint64_t _hard_limit;
  uint64_t _allocated;

  sdb_mutex_t *_lock;
} sdb_cache_manager_t;

sdb_cache_manager_t *sdb_cache_manager_create(uint64_t soft_limit, uint64_t hard_limit);
void sdb_cache_manager_destroy(sdb_cache_manager_t *cache);
sdb_cache_consumer_t sdb_cache_manager_register_consumer(sdb_cache_manager_t *cache, void *consumer);
void sdb_cache_manager_allocate(sdb_cache_manager_t *cache, sdb_cache_consumer_t consumer, int memory_delta);
void sdb_cache_manager_update(sdb_cache_manager_t *cache, sdb_cache_consumer_t consumer);

#endif  // SRC_STORAGE_CACHE_MANAGER_H_
