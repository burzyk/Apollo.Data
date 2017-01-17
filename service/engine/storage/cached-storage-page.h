//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_CACHED_STORAGE_PAGE_H
#define APOLLO_STORAGE_CACHED_STORAGE_PAGE_H

#include <cstdio>
#include "storage-page.h"
#define A_PAGE_LOAD_BUFFER_SIZE 65536

namespace apollo {

class CachedStoragePage : public StoragePage {
 public:
  virtual ~CachedStoragePage();
  static CachedStoragePage *Load(FILE *f, uint64_t size, uint64_t file_offset);

  virtual void Write(uint64_t offset, void *source, uint64_t size);
  virtual volatile_t *Read(uint64_t offset, uint64_t size);

  virtual uint64_t GetPageSize();
 private:
  CachedStoragePage(FILE *f, uint64_t size, uint64_t file_offset);

  uint8_t *content;
  uint64_t size;
  FILE *file;
  uint64_t file_offset;
};

}

#endif //APOLLO_STORAGE_CACHEDSTORAGEPAGE_H
