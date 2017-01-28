//
// Created by Pawel Burzynski on 18/01/2017.
//

#include <src/fatal-exception.h>
#include "file.h"
#include "log.h"

namespace apollo {

File::File(std::string file_name) {
  this->f = fopen(file_name.c_str(), "rb+");

  if (this->f == nullptr) {
    this->f = fopen(file_name.c_str(), "wb+");
  }

  if (this->f == nullptr) {
    throw FatalException("Unable to open file");
  }
}

File::~File() {
  if (this->f != nullptr) {
    fclose(this->f);
    this->f = nullptr;
  }
}

void File::Write(void *buffer, size_t size, size_t count) {
  if (fwrite(buffer, size, count, this->f) != count) {
    throw FatalException("Wrote less than expected");
  }
}

size_t File::Read(void *buffer, size_t size, size_t count) {
  // TODO: semaphore for amount of opened files

  return fread(buffer, size, count, this->f);
}

void File::Seek(off_t offset, int origin) {
  if (fseek(this->f, offset, origin) == -1) {
    throw FatalException("Unable to seek in the file");
  }
}

size_t File::GetSize() {
  long position = ftell(this->f);
  this->Seek(0, SEEK_END);
  long size = ftell(this->f);
  this->Seek(position, SEEK_SET);

  return size;
}

void File::Flush() {
  fflush(this->f);
}

}