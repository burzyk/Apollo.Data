/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//
// Created by Pawel Burzynski on 18/01/2017.
//

#include <src/fatal-exception.h>
#include "file.h"
#include "src/log.h"

namespace shakadb {

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

void File::Write(void *buffer, size_t size) {
  if (fwrite(buffer, 1, size, this->f) != size) {
    throw FatalException("Wrote less than expected");
  }
}

size_t File::Read(void *buffer, size_t size) {
  // TODO: semaphore for amount of opened files

  return fread(buffer, 1, size, this->f);
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