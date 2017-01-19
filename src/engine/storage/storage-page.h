//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_STORAGE_PAGE_H
#define APOLLO_STORAGE_STORAGE_PAGE_H

#include <cstdint>

namespace apollo {

typedef uint8_t volatile_t;

class StoragePage {
 public:
  virtual ~StoragePage() {};

  virtual void Write(int offset, void *source, int size) = 0;
  virtual volatile_t *Read(int offset, int size) = 0;

  virtual int GetPageSize() = 0;
};

}

#endif //APOLLO_STORAGE_STORAGE_PAGE_H
