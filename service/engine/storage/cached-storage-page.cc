//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <utils/common.h>
#include "utils/log.h"
#include "cached-storage-page.h"

namespace apollo {

CachedStoragePage *CachedStoragePage::Load(FILE *f, int size, uint64_t file_offset) {
  CachedStoragePage *page = new CachedStoragePage(f, size, file_offset);
  uint64_t total_read = 0;
  size_t read = (size_t)-1;

  fseek(f, (off_t)file_offset, SEEK_SET);

  while (read != 0 && size != 0) {
    read = fread(page->content + total_read, 1, (size_t)(MIN(size, A_PAGE_LOAD_BUFFER_SIZE)), f);
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

void CachedStoragePage::Write(int offset, void *source, int size) {
  if (this->size < offset + size) {
    Log::Fatal("Trying to write outside page");
  }

  fseek(this->file, (off_t)this->file_offset + offset, SEEK_SET);
  if (fwrite(source, 1, (size_t)size, this->file) != size) {
    Log::Fatal("Write not finished writing all bytes");
  }
  memcpy(this->content + offset, source, (size_t)size);
}

volatile_t *CachedStoragePage::Read(int offset, int size) {
  if (this->size < offset + size) {
    Log::Fatal("Trying to read outside page");
  }

  return this->content + offset;
}

int CachedStoragePage::GetPageSize() {
  return this->size;
}

CachedStoragePage::CachedStoragePage(FILE *f, int size, uint64_t file_offset) {
  this->content = (uint8_t *)calloc((size_t)size, 1);
  this->size = size;
  this->file = f;
  this->file_offset = file_offset;
}

}