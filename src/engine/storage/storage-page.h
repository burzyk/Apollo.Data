//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_STORAGE_PAGE_H
#define APOLLO_STORAGE_STORAGE_PAGE_H

#include <cstdint>

namespace apollo {

class StoragePage {
 public:
  virtual ~StoragePage() {};

  virtual void Write(int offset, void *source, int bytes_count) = 0;
  virtual int Read(int offset, void *buffer, int bytes_count) = 0;

  virtual int GetPageSize() = 0;
};

}

#endif //APOLLO_STORAGE_STORAGE_PAGE_H
