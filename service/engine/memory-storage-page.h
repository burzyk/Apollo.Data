//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_MEMORY_STORAGE_PAGE_H
#define APOLLO_STORAGE_MEMORY_STORAGE_PAGE_H

#include <cstdint>
#include "storage-page.h"

namespace apollo {

class MemoryStoragePage : public StoragePage {
 public:
  MemoryStoragePage(uint64_t size);
  virtual ~MemoryStoragePage();

  virtual void Write(uint64_t offset, void *source, uint64_t size);
  virtual volatile_t *Read(uint64_t offset, uint64_t size);

  virtual uint64_t GetPageSize();
 private:
  uint8_t *page;
  uint64_t size;
};

}

#endif //APOLLO_STORAGE_MEMORY_STORAGE_PAGE_H
