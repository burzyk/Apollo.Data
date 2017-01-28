//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_CACHED_STORAGE_PAGE_H
#define APOLLO_STORAGE_CACHED_STORAGE_PAGE_H

#include <cstdio>
#include <src/utils/file.h>
#include "storage-page.h"
#define A_PAGE_LOAD_BUFFER_SIZE 65536

namespace apollo {

class CachedStoragePage : public StoragePage {
 public:
  CachedStoragePage(std::string file_name, int size, uint64_t file_offset);

  virtual void Write(int offset, void *source, int bytes_count);
  virtual int Read(int offset, void *buffer, int bytes_count);

  virtual int GetPageSize();
 private:
  void LoadToBuffer(uint8_t *buffer);

  std::string file_name;
  int size;
  uint64_t file_offset;
};

}

#endif //APOLLO_STORAGE_CACHEDSTORAGEPAGE_H
