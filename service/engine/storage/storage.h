//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_STORAGE_H
#define APOLLO_STORAGE_STORAGE_H

#include <cstdint>
#include "storage-page.h"

namespace apollo {

class Storage {
 public:
  virtual ~Storage() {};
  virtual StoragePage *AllocatePage() = 0;
  virtual StoragePage *GetPage(uint64_t index) = 0;
  virtual uint64_t GetPagesCount() = 0;
  virtual void Flush() = 0;
};

}

#endif //APOLLO_STORAGE_STORAGE_H
