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
  static CachedStoragePage *Load(FILE *f, int size, uint64_t file_offset);

  virtual void Write(int offset, void *source, int size);
  virtual volatile_t *Read(int offset, int size);

  virtual int GetPageSize();
 private:
  CachedStoragePage(FILE *f, int size, uint64_t file_offset);

  uint8_t *content;
  int size;
  FILE *file;
  uint64_t file_offset;
};

}

#endif //APOLLO_STORAGE_CACHEDSTORAGEPAGE_H
