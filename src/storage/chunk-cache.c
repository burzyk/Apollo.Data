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

#include "src/storage/chunk-cache.h"

#include <inttypes.h>

#include "src/diagnostics.h"
#include "src/storage/data-chunk.h"

void cache_cleanup(chunk_cache_t *cache, cache_entry_t *reason);
void chunk_cache_insert_entry(chunk_cache_t *cache, cache_entry_t *entry);
void chunk_cache_cut_entry(chunk_cache_t *cache, cache_entry_t *entry);

chunk_cache_t *chunk_cache_create(uint64_t soft_limit, uint64_t hard_limit) {
  chunk_cache_t *cache = (chunk_cache_t *)sdb_alloc(sizeof(chunk_cache_t));
  cache->allocated = 0;
  cache->soft_limit = soft_limit;
  cache->hard_limit = hard_limit;
  cache->guard.consumer = NULL;
  cache->guard.allocated = 0;
  cache->guard.prev = &cache->guard;
  cache->guard.next = &cache->guard;

  if (soft_limit > hard_limit) {
    die("soft limit cannot be greater than hard limit");
  }

  return cache;
}

void chunk_cache_destroy(chunk_cache_t *cache) {
  cache_entry_t *curr = cache->guard.next;

  while (curr->consumer != NULL) {
    cache_entry_t *next = curr->next;
    sdb_free(curr);
    curr = next;
  }

  sdb_free(cache);
}

cache_entry_t *chunk_cache_register_consumer(chunk_cache_t *cache, void *consumer, uint64_t memory) {
  cache_entry_t *entry = (cache_entry_t *)sdb_alloc(sizeof(cache_entry_t));
  entry->allocated = 0;
  entry->consumer = consumer;

  chunk_cache_insert_entry(cache, entry);

  cache->allocated += memory;
  entry->allocated += memory;

  if (cache->soft_limit < cache->allocated && cache->allocated < cache->hard_limit) {
    log_debug("Soft cache limit reached with %" PRIu64 " bytes allocated", cache->allocated);
  }

  if (cache->hard_limit < cache->allocated) {
    log_info("Hard cache limit reached with %" PRIu64 " bytes allocated", cache->allocated);
    cache_cleanup(cache, entry);
  }

  return entry;
}

void chunk_cache_update(chunk_cache_t *cache, cache_entry_t *entry) {
  if (entry == NULL) {
    return;
  }

  chunk_cache_cut_entry(cache, entry);
  chunk_cache_insert_entry(cache, entry);
}

void chunk_cache_insert_entry(chunk_cache_t *cache, cache_entry_t *entry) {
  entry->next = cache->guard.next;
  entry->prev = &cache->guard;
  cache->guard.next->prev = entry;
  cache->guard.next = entry;

  cache->allocated += entry->allocated;
}

void chunk_cache_cut_entry(chunk_cache_t *cache, cache_entry_t *entry) {
  entry->next->prev = entry->prev;
  entry->prev->next = entry->next;

  cache->allocated -= entry->allocated;
}

void cache_cleanup(chunk_cache_t *cache, cache_entry_t *reason) {
  stopwatch_t *sw = stopwatch_start();
  log_info("Starting cache cleanup ...");
  cache_entry_t *curr = cache->guard.prev;
  int purged = 0;

  while (cache->allocated > cache->soft_limit && curr != &cache->guard) {
    if (curr != reason) {
      chunk_cache_cut_entry(cache, curr);
      sdb_data_chunk_clean_cache((sdb_data_chunk_t *)curr->consumer);

      cache_entry_t *next = curr->prev;
      sdb_free(curr);
      purged++;
      curr = next;
    } else {
      curr = curr->next;
    }
  }

  log_info("Cache cleaned up in %fs, removed: %d entries", stopwatch_stop_and_destroy(sw), purged);
}