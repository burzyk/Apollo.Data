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

#include "src/utils/memory.h"

#include <string.h>
#include <stdlib.h>

void *sdb_alloc(size_t size) {
  return calloc(1, size);
}

void *sdb_realloc(void *buffer, size_t size) {
  return realloc(buffer, size);
}

void sdb_free(void *buffer) {
  free(buffer);
}

int sdb_binary_reader_can_read(sdb_binary_reader_t *reader, size_t size) {
  if (!reader->success) {
    return 0;
  }

  return reader->success = ((char *)reader->_current - (char *)reader->_buffer) + size <= reader->_size;
}

void sdb_binary_reader_init(sdb_binary_reader_t *reader, void *buffer, size_t size) {
  reader->_buffer = buffer;
  reader->_current = buffer;
  reader->_size = size;
  reader->success = 1;
}

void sdb_binary_reader_read(sdb_binary_reader_t *reader, void *buffer, size_t size) {
  if (!sdb_binary_reader_can_read(reader, size)) {
    return;
  }

  memcpy(buffer, reader->_current, size);
  reader->_current = (char *)reader->_current + size;
}

void sdb_binary_reader_read_pointer(sdb_binary_reader_t *reader, void *buffer, size_t size) {
  if (!sdb_binary_reader_can_read(reader, size)) {
    return;
  }

  memcpy(buffer, &reader->_current, sizeof(reader->_current));
  reader->_current = (char *)reader->_current + size;
}