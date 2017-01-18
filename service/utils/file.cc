//
// Created by Pawel Burzynski on 18/01/2017.
//

#include "file.h"
#include "log.h"

namespace apollo {

File::File(std::string file_name) {
  this->f = fopen(file_name.c_str(), "rb+");

  if (this->f == NULL) {
    this->f = fopen(file_name.c_str(), "wb+");
  }

  if (this->f == NULL) {
    Log::Fatal("Unable to open file");
  }
}

File::~File() {
  if (this->f != NULL) {
    fclose(this->f);
    this->f = NULL;
  }
}

void File::Write(void *buffer, size_t size, size_t count) {
  if (fwrite(buffer, size, count, this->f) != count) {
    Log::Fatal("Wrote less than expected");
  }
}

size_t File::Read(void *buffer, size_t size, size_t count) {
  return fread(buffer, size, count, this->f);
}

void File::Seek(off_t offset, int origin) {
  if (fseek(this->f, offset, origin) == -1) {
    Log::Fatal("Unable to seek in the file");
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