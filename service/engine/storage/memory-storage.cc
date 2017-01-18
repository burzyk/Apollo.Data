//
// Created by Pawel Burzynski on 17/01/2017.
//

#include "memory-storage.h"

namespace apollo {

MemoryStorage::MemoryStorage(int page_size) {
  this->page_size = page_size;
}

MemoryStorage::~MemoryStorage() {
  for (auto p: this->pages) {
    delete p;
  }
}

StoragePage *MemoryStorage::AllocatePage() {
  MemoryStoragePage *page = new MemoryStoragePage(this->page_size);
  this->pages.push_back(page);

  return page;
}

StoragePage *MemoryStorage::GetPage(int index) {
  return this->pages[index];
}

int MemoryStorage::GetPagesCount() {
  return (int)this->pages.size();
}

void MemoryStorage::Flush() {
}

}