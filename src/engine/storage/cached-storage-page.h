//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_CACHED_STORAGE_PAGE_H
#define APOLLO_STORAGE_CACHED_STORAGE_PAGE_H

#include <cstdio>
#include <src/utils/file.h>
#include "storage-page.h"
#include "page-allocator.h"
#define A_PAGE_LOAD_BUFFER_SIZE 65536

namespace apollo {

class CachedStoragePage : public StoragePage {
 public:
  CachedStoragePage(File *file, int size, uint64_t file_offset, PageAllocator *allocator);

  virtual void Write(int offset, void *source, int size);
  virtual volatile_t *Read(int offset, int size);

  virtual int GetPageSize();
 private:
  void LoadToBuffer(uint8_t *buffer);

  PageAllocator *allocator;
  page_id_t page_id;
  int size;
  File *file;
  uint64_t file_offset;
};

}

#endif //APOLLO_STORAGE_CACHEDSTORAGEPAGE_H
