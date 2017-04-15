//
// Created by Pawel Burzynski on 14/04/2017.
//


#ifndef SRC_UTILS_MEMORY_H_
#define SRC_UTILS_MEMORY_H_

#include <stdio.h>

void *sdb_alloc(size_t size);
void *sdb_realloc(void *buffer, size_t size);
void sdb_free(void *buffer);

typedef struct sdb_binary_reader_s {
  void *buffer;
  void *current;
  size_t size;
  int success;
} sdb_binary_reader_t;

void sdb_binary_reader_init(sdb_binary_reader_t *reader, void *buffer, size_t size);
void sdb_binary_reader_read(sdb_binary_reader_t *reader, void *buffer, size_t size);
void sdb_binary_reader_read_pointer(sdb_binary_reader_t *reader, void *buffer, size_t size);

#endif  // SRC_UTILS_MEMORY_H_
