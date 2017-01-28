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
  virtual StoragePage *GetPage(int index) = 0;
  virtual int GetPagesCount() = 0;
};

}

#endif //APOLLO_STORAGE_STORAGE_H
