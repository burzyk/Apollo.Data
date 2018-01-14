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

#ifndef SRC_STORAGE_CACHE_H_
#define SRC_STORAGE_CACHE_H_

#include <stdint.h>

typedef struct cache_entry_s {
  uint64_t allocated;
  void *consumer;
  struct cache_entry_s *prev;
  struct cache_entry_s *next;
} cache_entry_t;

typedef struct chunk_cache_s {
  uint64_t soft_limit;
  uint64_t hard_limit;
  uint64_t allocated;

  cache_entry_t guard;
} chunk_cache_t;

chunk_cache_t *chunk_cache_create(uint64_t soft_limit, uint64_t hard_limit);
void chunk_cache_destroy(chunk_cache_t *cache);
cache_entry_t *chunk_cache_register_consumer(chunk_cache_t *cache, void *consumer, uint64_t memory);
void chunk_cache_update(chunk_cache_t *cache, cache_entry_t *entry);

#endif  // SRC_STORAGE_CACHE_H_
