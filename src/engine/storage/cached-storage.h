//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_CACHED_STORAGE_H
#define APOLLO_STORAGE_CACHED_STORAGE_H

#include <string>
#include <vector>
#include <src/utils/file.h>
#include "storage.h"
#include "cached-storage-page.h"

namespace apollo {

class CachedStorage : public Storage {
 public:
  virtual ~CachedStorage();
  static CachedStorage *Init(std::string file_name, int page_size, int max_pages);

  virtual StoragePage *AllocatePage();
  virtual StoragePage *GetPage(int index);
  virtual int GetPagesCount();
  virtual void Flush();
 private:
  CachedStorage(File *file, MemoryPageAllocator *allocator, int page_size);

  int page_size;
  File *file;
  MemoryPageAllocator *allocator;
  std::vector<CachedStoragePage *> pages;
};

}

#endif //APOLLO_STORAGE_CACHEDSTORAGE_H
