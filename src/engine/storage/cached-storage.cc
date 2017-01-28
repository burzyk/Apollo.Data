//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <src/utils/common.h>
#include "src/utils/log.h"
#include "cached-storage.h"

#define A_PAGE_ALLOCATE_BUFFER_SIZE 65536

namespace apollo {

CachedStorage *CachedStorage::Init(std::string file_name, int page_size, int max_pages) {
  File *f = new File(file_name);
  MemoryPageAllocator *allocator = new MemoryPageAllocator((size_t)page_size, max_pages);
  CachedStorage *storage = new CachedStorage(f, allocator, page_size);

  long pages_count = f->GetSize() / page_size;

  for (int i = 0; i < pages_count; i++) {
    storage->pages.push_back(new CachedStoragePage(f, page_size, (uint64_t)i * page_size, allocator));
  }

  return storage;
}

CachedStorage::~CachedStorage() {
  for (auto p: this->pages) {
    delete p;
  }

  this->pages.clear();

  delete this->file;
  delete this->allocator;
}

StoragePage *CachedStorage::AllocatePage() {
  uint8_t buffer[A_PAGE_ALLOCATE_BUFFER_SIZE] = {0};
  int to_allocate = this->page_size;

  this->file->Seek(0, SEEK_END);

  while (to_allocate > 0) {
    int to_write = MIN(to_allocate, A_PAGE_ALLOCATE_BUFFER_SIZE);
    this->file->Write(buffer, (size_t)to_write, 1);
    to_allocate -= to_write;
  }

  this->file->Flush();

  CachedStoragePage *page = new CachedStoragePage(
      this->file,
      this->page_size,
      (uint64_t)this->page_size * this->GetPagesCount(),
      this->allocator);
  this->pages.push_back(page);

  return page;
}

StoragePage *CachedStorage::GetPage(int index) {
  return this->pages[index];
}

int CachedStorage::GetPagesCount() {
  return (int)this->pages.size();
}

void CachedStorage::Flush() {
  this->file->Flush();
}

CachedStorage::CachedStorage(File *file, MemoryPageAllocator *allocator, int page_size) {
  this->allocator = allocator;
  this->page_size = page_size;
  this->file = file;
}

}