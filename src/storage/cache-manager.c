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

#include "src/diagnostics.h"
#include "src/storage/data-chunk.h"

void sdb_cache_manager_cleanup(sdb_cache_manager_t *cache, sdb_cache_entry_t *reason);
void sdb_cache_manager_insert_entry(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry);
void sdb_cache_manager_cut_entry(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry);

sdb_cache_manager_t *sdb_cache_manager_create(uint64_t soft_limit, uint64_t hard_limit) {
  sdb_cache_manager_t *cache = (sdb_cache_manager_t *)sdb_alloc(sizeof(sdb_cache_manager_t));
  cache->_allocated = 0;
  cache->soft_limit = soft_limit;
  cache->hard_limit = hard_limit;
  cache->_guard.consumer = NULL;
  cache->_guard.allocated = 0;
  cache->_guard.prev = &cache->_guard;
  cache->_guard.next = &cache->_guard;

  if (soft_limit > hard_limit) {
    die("soft limit cannot be greater than hard limit");
  }

  return cache;
}

void sdb_cache_manager_destroy(sdb_cache_manager_t *cache) {
  sdb_cache_entry_t *curr = cache->_guard.next;

  while (curr->consumer != NULL) {
    sdb_cache_entry_t *next = curr->next;
    sdb_free(curr);
    curr = next;
  }

  sdb_free(cache);
}

sdb_cache_entry_t *sdb_cache_manager_register_consumer(sdb_cache_manager_t *cache, void *consumer, uint64_t memory) {
  sdb_cache_entry_t *entry = (sdb_cache_entry_t *)sdb_alloc(sizeof(sdb_cache_entry_t));
  entry->allocated = 0;
  entry->consumer = consumer;

  sdb_cache_manager_insert_entry(cache, entry);

  cache->_allocated += memory;
  entry->allocated += memory;

  if (cache->soft_limit < cache->_allocated && cache->_allocated < cache->hard_limit) {
    log_debug("Soft cache limit reached with %" PRIu64 " bytes allocated", cache->_allocated);
  }

  if (cache->hard_limit < cache->_allocated) {
    log_info("Hard cache limit reached with %" PRIu64 " bytes allocated", cache->_allocated);
    sdb_cache_manager_cleanup(cache, entry);
  }

  return entry;
}

void sdb_cache_manager_update(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry) {
  if (entry == NULL) {
    return;
  }

  sdb_cache_manager_cut_entry(cache, entry);
  sdb_cache_manager_insert_entry(cache, entry);
}

void sdb_cache_manager_insert_entry(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry) {
  entry->next = cache->_guard.next;
  entry->prev = &cache->_guard;
  cache->_guard.next->prev = entry;
  cache->_guard.next = entry;

  cache->_allocated += entry->allocated;
}

void sdb_cache_manager_cut_entry(sdb_cache_manager_t *cache, sdb_cache_entry_t *entry) {
  entry->next->prev = entry->prev;
  entry->prev->next = entry->next;

  cache->_allocated -= entry->allocated;
}

void sdb_cache_manager_cleanup(sdb_cache_manager_t *cache, sdb_cache_entry_t *reason) {
  stopwatch_t *sw = stopwatch_start();
  log_info("Starting cache cleanup ...");
  sdb_cache_entry_t *curr = cache->_guard.prev;
  int purged = 0;

  while (cache->_allocated > cache->soft_limit && curr != &cache->_guard) {
    if (curr != reason) {
      sdb_cache_manager_cut_entry(cache, curr);
      sdb_data_chunk_clean_cache((sdb_data_chunk_t *)curr->consumer);

      sdb_cache_entry_t *next = curr->prev;
      sdb_free(curr);
      purged++;
      curr = next;
    } else {
      curr = curr->next;
    }
  }

  log_info("Cache cleaned up in %fs, removed: %d entries", stopwatch_stop_and_destroy(sw), purged);
}