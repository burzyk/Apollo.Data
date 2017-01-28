//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <src/utils/common.h>
#include <src/fatal-exception.h>
#include "src/utils/log.h"
#include "cached-storage-page.h"

namespace apollo {

CachedStoragePage::CachedStoragePage(std::string file_name, int size, uint64_t file_offset) {
  this->size = size;
  this->file_name = file_name;
  this->file_offset = file_offset;
}

void CachedStoragePage::LoadToBuffer(uint8_t *buffer) {
  int total_read = 0;
  size_t read = (size_t)-1;
  int to_read = this->size;
  File file(this->file_name);

  file.Flush();
  file.Seek((off_t)this->file_offset, SEEK_SET);

  while (read != 0 && to_read != 0) {
    read = file.Read(buffer + total_read, 1, (size_t)(MIN(to_read, A_PAGE_LOAD_BUFFER_SIZE)));
    total_read += read;
    to_read -= read;
  }
}

void CachedStoragePage::Write(int offset, void *source, int bytes_count) {
  if (bytes_count == 0) {
    return;
  }

  if (this->size < offset + bytes_count) {
    throw FatalException("Trying to write outside page");
  }

  File file(this->file_name);

  file.Seek((off_t)this->file_offset + offset, SEEK_SET);
  file.Write(source, 1, (size_t)bytes_count);

//  uint8_t *content = this->allocator->GetPage(this->page_id);
//
//  if (content != nullptr) {
//    memcpy(content + offset, source, (size_t)bytes_count);
//  }
}

int CachedStoragePage::Read(int offset, void *buffer, int bytes_count) {
  if (bytes_count == 0) {
    return 0;
  }

  if (this->size < offset + bytes_count) {
    throw FatalException("Trying to read outside page");
  }

  //uint8_t *content = this->allocator->GetPage(this->page_id);

//  if (content == nullptr) {
//    this->page_id = this->allocator->AllocatePage();
//    content = this->allocator->GetPage(this->page_id);
//    this->LoadToBuffer(content);
//  }
//
//  memcpy(buffer, content + offset, bytes_count);
  return bytes_count;
}

int CachedStoragePage::GetPageSize() {
  return this->size;
}

}