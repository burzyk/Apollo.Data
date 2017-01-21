//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_MEMORY_STORAGE_PAGE_H
#define APOLLO_STORAGE_MEMORY_STORAGE_PAGE_H

#include <cstdint>
#include "src/engine/storage/storage-page.h"

namespace apollo {

class MemoryStoragePage : public StoragePage {
 public:
  MemoryStoragePage(int size);
  virtual ~MemoryStoragePage();

  virtual void Write(int offset, void *source, int size);
  virtual volatile_t *Read(int offset, int size);

  virtual int GetPageSize();
 private:
  uint8_t *page;
  int size;
};

}

#endif //APOLLO_STORAGE_MEMORY_STORAGE_PAGE_H
