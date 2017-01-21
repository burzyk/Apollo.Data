//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <src/utils/common.h>
#include "src/utils/log.h"
#include "cached-storage-page.h"

namespace apollo {

CachedStoragePage::CachedStoragePage(File *file, int size, uint64_t file_offset, PageAllocator *allocator) {
  this->allocator = allocator;
  this->size = size;
  this->file = file;
  this->file_offset = file_offset;
}

void CachedStoragePage::LoadToBuffer(uint8_t *buffer) {
  int total_read = 0;
  size_t read = (size_t)-1;
  int to_read = this->size;

  this->file->Seek((off_t)this->file_offset, SEEK_SET);

  while (read != 0 && to_read != 0) {
    read = this->file->Read(buffer + total_read, 1, (size_t)(MIN(to_read, A_PAGE_LOAD_BUFFER_SIZE)));
    total_read += read;
    to_read -= read;
  }
}

void CachedStoragePage::Write(int offset, void *source, int size) {
  if (size == 0) {
    return;
  }

  if (this->size < offset + size) {
    Log::Fatal("Trying to write outside page");
  }

  this->file->Seek((off_t)this->file_offset + offset, SEEK_SET);
  this->file->Write(source, 1, (size_t)size);

  uint8_t *content = this->allocator->GetPage(this->page_id);

  if (content != NULL) {
    memcpy(content + offset, source, (size_t)size);
  }

}

volatile_t *CachedStoragePage::Read(int offset, int size) {
  if (this->size < offset + size) {
    Log::Fatal("Trying to read outside page");
  }

  uint8_t *content = this->allocator->GetPage(this->page_id);

  if (content == NULL) {
    this->page_id = this->allocator->AllocatePage();
    content = this->allocator->GetPage(this->page_id);
    this->LoadToBuffer(content);
  }

  return content + offset;
}

int CachedStoragePage::GetPageSize() {
  return this->size;
}

}