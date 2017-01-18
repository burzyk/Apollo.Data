//
// Created by Pawel Burzynski on 17/01/2017.
//

#include "utils/log.h"
#include "cached-storage.h"

namespace apollo {

CachedStorage *CachedStorage::Init(std::string file_name, int page_size) {
  FILE *f = fopen(file_name.c_str(), "ab+");

  if (f == NULL) {
    Log::Fatal("Unable to open data file");
  }

  CachedStorage *storage = new CachedStorage(f, page_size);

  fseek(f, 0, SEEK_END);
  long pages_count = ftell(f) / page_size + (ftell(f) % page_size ? 1 : 0);
  fseek(f, 0, SEEK_SET);

  for (int i = 0; i < pages_count; i++) {
    storage->pages.push_back(CachedStoragePage::Load(f, page_size, (uint64_t )i * page_size));
  }

  return storage;
}

CachedStorage::~CachedStorage() {
  for (auto p: this->pages) {
    delete p;
  }

  this->pages.clear();

  if (this->file != NULL) {
    fclose(this->file);
    this->file = NULL;
  }
}

StoragePage *CachedStorage::AllocatePage() {
  CachedStoragePage *page = CachedStoragePage::Load(
      this->file,
      this->page_size,
      (uint64_t)this->page_size * this->GetPagesCount());
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
  fflush(this->file);
}

CachedStorage::CachedStorage(FILE *f, int page_size) {
  this->page_size = page_size;
  this->file = f;
}

}