//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_MEMORY_STORAGE_H
#define APOLLO_STORAGE_MEMORY_STORAGE_H

#include <vector>
#include <cstdint>
#include "src/engine/storage/storage.h"
#include "memory-storage-page.h"

namespace apollo {

class MemoryStorage : public Storage {
 public:
  MemoryStorage(int page_size);
  virtual ~MemoryStorage();

  virtual StoragePage *AllocatePage();
  virtual StoragePage *GetPage(int index);
  virtual int GetPagesCount();
  virtual void Flush();
 private:
  int page_size;
  std::vector<MemoryStoragePage *> pages;
};

}

#endif //APOLLO_STORAGE_MEMORYSTORAGE_H
