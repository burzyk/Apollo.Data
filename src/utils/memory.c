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

  return reader->success = ((char *)reader->current - (char *)reader->buffer) + size <= reader->size;
}

void sdb_binary_reader_init(sdb_binary_reader_t *reader, void *buffer, size_t size) {
  reader->buffer = buffer;
  reader->current = buffer;
  reader->size = size;
  reader->success = 1;
}

void sdb_binary_reader_read(sdb_binary_reader_t *reader, void *buffer, size_t size) {
  if (!sdb_binary_reader_can_read(reader, size)) {
    return;
  }

  memcpy(buffer, reader->current, size);
  reader->current = (char *)reader->current + size;
}

void sdb_binary_reader_read_pointer(sdb_binary_reader_t *reader, void *buffer, size_t size) {
  if (!sdb_binary_reader_can_read(reader, size)) {
    return;
  }

  memcpy(&buffer, &reader->current, sizeof(reader->current));
  reader->current = (char *)reader->current + size;
}