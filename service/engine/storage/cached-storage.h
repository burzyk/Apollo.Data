//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_CACHED_STORAGE_H
#define APOLLO_STORAGE_CACHED_STORAGE_H

#include <string>
#include <vector>
#include "storage.h"
#include "cached-storage-page.h"

namespace apollo {

class CachedStorage : public Storage {
 public:
  virtual ~CachedStorage();
  static CachedStorage *Init(std::string file_name, uint64_t page_size);

  virtual StoragePage *AllocatePage();
  virtual StoragePage *GetPage(uint64_t index);
  virtual uint64_t GetPagesCount();
  virtual void Flush();
 private:
  CachedStorage(FILE *f, uint64_t page_size);

  uint64_t page_size;
  FILE *file;
  std::vector<CachedStoragePage *> pages;
};

}

#endif //APOLLO_STORAGE_CACHEDSTORAGE_H
