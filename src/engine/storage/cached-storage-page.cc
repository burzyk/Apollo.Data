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
  this->cached_file = nullptr;
}

CachedStoragePage::~CachedStoragePage() {
  if (this->cached_file != nullptr) {
    free(this->cached_file);
    this->cached_file = nullptr;
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
  file.Write(source, (size_t)bytes_count);

  if (this->cached_file != nullptr) {
    memcpy(this->cached_file + offset, source, (size_t)bytes_count);
  }
}

int CachedStoragePage::Read(int offset, void *buffer, int bytes_count) {
  if (bytes_count == 0) {
    return 0;
  }

  if (this->size < offset + bytes_count) {
    throw FatalException("Trying to read outside page");
  }

  if (this->cached_file == nullptr) {
    this->Load();
  }

  memcpy(buffer, this->cached_file + offset, (size_t)bytes_count);

  return bytes_count;
}

int CachedStoragePage::GetPageSize() {
  return this->size;
}

void CachedStoragePage::Load() {
  if (this->cached_file != nullptr) {
    throw FatalException("Cached file is already allocated");
  }

  int total_read = 0;
  size_t read = (size_t)-1;

  this->cached_file = (uint8_t *)calloc((size_t)this->size, 1);

  File file(this->file_name);
  file.Seek((off_t)this->file_offset, SEEK_SET);

  while (read != 0 && total_read < this->size) {
    read = file.Read(
        this->cached_file + total_read,
        (size_t)(MIN(this->size - total_read, A_PAGE_LOAD_BUFFER_SIZE)));
    total_read += read;
  }
}

}