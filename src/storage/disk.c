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

#include "disk.h"

#include <unistd.h>
#include <sys/mman.h>
#include <src/common.h>

FILE *file_open(const char *file_name);

file_map_t *file_map_create(const char *file_name) {
  FILE *f = file_open(file_name);

  if (f == NULL) {
    return NULL;
  }

  file_map_t *map = (file_map_t *)sdb_alloc(sizeof(file_map_t));
  map->size = (uint64_t)ftello(f);
  map->data = map->size == 0 ? NULL : mmap(NULL, map->size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(f), 0);

  fclose(f);

  if (map->size != 0 && map->data == NULL) {
    die("Failed to map memory");
  }

  return map;
}

void file_map_destroy(file_map_t *file) {
  if (file->data != NULL) {
    munmap(file->data, file->size);
  }

  sdb_free(file);
}

void file_map_sync(file_map_t *file) {
  if (file->data == NULL) {
    return;
  }

  if (msync(file->data, file->size, MS_SYNC)) {
    die("Failed to synchronize the file");
  }
}

void file_grow(const char *file_name, uint64_t increment) {
  uint8_t *buffer = (uint8_t *)sdb_alloc(SDB_GROW_BUFFER_SIZE);
  FILE *f = file_open(file_name);

  if (f == NULL) {
    die("Failed to grow the file");
  }

  while (increment > 0) {
    increment -= fwrite(buffer, 1, sdb_minl(SDB_GROW_BUFFER_SIZE, increment), f);
  }

  fclose(f);
  sdb_free(buffer);
}

void file_unlink(const char *file_name) {
  unlink(file_name);
}

FILE *file_open(const char *file_name) {
  return fopen(file_name, "a+");
}
