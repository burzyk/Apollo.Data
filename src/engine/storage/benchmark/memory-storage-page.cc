//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cstdlib>
#include "memory-storage-page.h"
#include "src/utils/log.h"

namespace apollo {

MemoryStoragePage::MemoryStoragePage(int size) {
  this->page = (uint8_t *)calloc((size_t)size, 1);
  this->size = size;
}

MemoryStoragePage::~MemoryStoragePage() {
  if (this->page != nullptr) {
    free(this->page);
  }

  this->page = nullptr;
  this->size = 0;
}

void MemoryStoragePage::Write(int offset, void *source, int size) {
  if (this->size < offset + size) {
    Log::Fatal("Trying to write outside page");
  }

  memcpy(this->page + offset, source, (size_t)size);
}

volatile_t *MemoryStoragePage::Read(int offset, int size) {
  if (this->size < offset + size) {
    Log::Fatal("Trying to read outside page");
  }

  return this->page + offset;
}

int MemoryStoragePage::GetPageSize() {
  return this->size;
}

}
