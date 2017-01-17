//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cmath>
#include <cstdlib>
#include <iostream>
#include "utils/log.h"
#include "cached-storage-page.h"

namespace apollo {

CachedStoragePage *CachedStoragePage::Load(FILE *f, uint64_t size, uint64_t file_offset) {
  CachedStoragePage *page = new CachedStoragePage(f, size, file_offset);
  uint64_t total_read = 0;
  size_t read = -1;

  fseek(f, file_offset, SEEK_SET);

  while (read != 0 && size != 0) {
    read = fread(page->content + total_read, 1, fmin(size, A_PAGE_LOAD_BUFFER_SIZE), f);
    total_read += read;
    size -= read;
  }

  return page;
}

CachedStoragePage::~CachedStoragePage() {
  if (this->content != NULL) {
    free(this->content);
  }

  this->content = NULL;
  this->size = 0;
}

void CachedStoragePage::Write(uint64_t offset, void *source, uint64_t size) {
  if (this->size < offset + size) {
    Log::Fatal("Trying to write outside page");
  }

  fseek(this->file, this->file_offset + offset, SEEK_SET);
  fwrite(source, 1, size, this->file);
  memcpy(this->content + offset, source, size);
}

volatile_t *CachedStoragePage::Read(uint64_t offset, uint64_t size) {
  if (this->size < offset + size) {
    Log::Fatal("Trying to read outside page");
  }

  return this->content + offset;
}

uint64_t CachedStoragePage::GetPageSize() {
  return this->size;
}

CachedStoragePage::CachedStoragePage(FILE *f, uint64_t size, uint64_t file_offset) {
  this->content = (uint8_t *)calloc(size, 1);
  this->size = size;
  this->file = f;
  this->file_offset = file_offset;
}

}