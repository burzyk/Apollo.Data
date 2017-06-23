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

#include "src/storage/cache-manager.h"

#include <inttypes.h>

#include "src/utils/diagnostics.h"
#include "src/utils/memory.h"
#include "src/storage/data-chunk.h"

void sdb_cache_manager_cleanup(sdb_cache_manager_t *cache);
void sdb_cache_manager_insert_entry(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry);
void sdb_cache_manager_cut_entry(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry);

sdb_cache_manager_t *sdb_cache_manager_create(uint64_t soft_limit, uint64_t hard_limit) {
  sdb_cache_manager_t *cache = (sdb_cache_manager_t *)sdb_alloc(sizeof(sdb_cache_manager_t));
  cache->_allocated = 0;
  cache->soft_limit = soft_limit;
  cache->hard_limit = hard_limit;
  cache->_lock = sdb_mutex_create();
  cache->_consumers_count = 0;
  cache->_guard.consumer = NULL;
  cache->_guard.allocated = 0;
  cache->_guard.prev = &cache->_guard;
  cache->_guard.next = &cache->_guard;

  return cache;
}

void sdb_cache_manager_destroy(sdb_cache_manager_t *cache) {
  sdb_cache_entry_t *curr = cache->_guard.next;

  while (curr->consumer != NULL) {
    sdb_cache_entry_t *next = curr->next;
    sdb_free(curr);
    curr = next;
  }

  sdb_mutex_destroy(cache->_lock);
  sdb_free(cache);
}

sdb_cache_entry_t *sdb_cache_manager_register_consumer(sdb_cache_manager_t *cache, void *consumer) {
  sdb_cache_entry_t *entry = (sdb_cache_entry_t *)sdb_alloc(sizeof(sdb_cache_entry_t));
  entry->allocated = 0;
  entry->consumer = consumer;

  sdb_mutex_lock(cache->_lock);
  sdb_cache_manager_insert_entry(cache, entry);
  sdb_mutex_unlock(cache->_lock);

  return entry;
}

void sdb_cache_manager_allocate(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry, uint64_t memory_delta) {
  sdb_mutex_lock(cache->_lock);
  cache->_allocated += memory_delta;
  entry->allocated += memory_delta;

  if (cache->soft_limit < cache->_allocated && cache->_allocated < cache->hard_limit) {
    sdb_log_debug("Soft cache limit reached with %" PRIu64 " bytes allocated", cache->_allocated);
  }

  if (cache->hard_limit < cache->_allocated) {
    sdb_log_info("Hard cache limit reached with %" PRIu64 " bytes allocated", cache->_allocated);
    sdb_cache_manager_cleanup(cache);
  }

  sdb_mutex_unlock(cache->_lock);
}

void sdb_cache_manager_update(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry) {
  sdb_mutex_lock(cache->_lock);

  sdb_cache_manager_cut_entry(cache, entry);
  sdb_cache_manager_insert_entry(cache, entry);

  sdb_mutex_unlock(cache->_lock);
}

void sdb_cache_manager_insert_entry(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry) {
  entry->next = cache->_guard.next;
  entry->prev = &cache->_guard;
  cache->_guard.next->prev = entry;
  cache->_guard.next = entry;

  cache->_consumers_count++;
  cache->_allocated += entry->allocated;
}

void sdb_cache_manager_cut_entry(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry) {
  entry->next->prev = entry->prev;
  entry->prev->next = entry->next;

  cache->_consumers_count--;
  cache->_allocated -= entry->allocated;
}

void sdb_cache_manager_cleanup(sdb_cache_manager_t *cache) {
  sdb_stopwatch_t *sw = sdb_stopwatch_start();
  sdb_log_info("Starting cache cleanup ...");
  sdb_cache_entry_t *curr = cache->_guard.prev;

  while (cache->_allocated > cache->soft_limit && cache->_consumers_count != 0) {
    sdb_cache_manager_cut_entry(cache, curr);
    sdb_data_chunk_clean_cache((sdb_data_chunk_t *)curr->consumer);

    sdb_cache_entry_t *next = curr->prev;
    sdb_free(curr);
    curr = next;
  }

  sdb_log_info("Cache cleaned up in %fs", sdb_stopwatch_stop_and_destroy(sw));
}