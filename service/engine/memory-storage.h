//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_MEMORY_STORAGE_H
#define APOLLO_STORAGE_MEMORY_STORAGE_H

#include <vector>
#include <cstdint>
#include "storage.h"
#include "memory-storage-page.h"

namespace apollo {

class MemoryStorage : public Storage {
 public:
  MemoryStorage(uint64_t page_size);
  virtual ~MemoryStorage();

  virtual StoragePage *AllocatePage();
  virtual StoragePage *GetPage(uint64_t index);
  virtual uint64_t GetPagesCount();
  virtual void Flush();
 private:
  uint64_t page_size;
  std::vector<MemoryStoragePage *> pages;
};

}

#endif //APOLLO_STORAGE_MEMORYSTORAGE_H
