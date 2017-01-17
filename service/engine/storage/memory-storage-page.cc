//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cstdlib>
#include "memory-storage-page.h"
#include "utils/log.h"

namespace apollo {

MemoryStoragePage::MemoryStoragePage(uint64_t size) {
  this->page = (uint8_t *)calloc(size, 1);
  this->size = size;
}

MemoryStoragePage::~MemoryStoragePage() {
  if (this->page != NULL) {
    free(this->page);
  }

  this->page = NULL;
  this->size = 0;
}

void MemoryStoragePage::Write(uint64_t offset, void *source, uint64_t size) {
  if (this->size < offset + size) {
    Log::Fatal("Trying to write outside page");
  }

  memcpy(this->page + offset, source, size);
}

volatile_t *MemoryStoragePage::Read(uint64_t offset, uint64_t size) {
  if (this->size < offset + size) {
    Log::Fatal("Trying to read outside page");
  }

  return this->page + offset;
}

uint64_t MemoryStoragePage::GetPageSize() {
  return this->size;
}

}
