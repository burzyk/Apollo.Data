//
// Created by Pawel Burzynski on 17/01/2017.
//

#include "memory-storage.h"

namespace apollo {

MemoryStorage::MemoryStorage(uint64_t page_size) {
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

StoragePage *MemoryStorage::GetPage(uint64_t index) {
  return this->pages[index];
}

uint64_t MemoryStorage::GetPagesCount() {
  return this->pages.size();
}

void MemoryStorage::Flush() {
}

}