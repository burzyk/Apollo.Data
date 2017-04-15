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
// Created by Pawel Burzynski on 14/04/2017.
//

#include "src/utils/disk.h"

#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

int sdb_directory_create(const char *directory_name) {
  int status = mkdir(directory_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  return status && errno != EEXIST;
}

sdb_file_t *sdb_file_open(const char *file_name) {
  sdb_file_t *f = fopen(file_name, "rb+");

  if (f == NULL) {
    f = fopen(file_name, "wb+");
  }

  return f;
}

void sdb_file_close(sdb_file_t *file) {
  fclose(file);
}

size_t sdb_file_write(sdb_file_t *file, void *buffer, size_t size) {
  return fwrite(buffer, size, 1, file);
}

size_t sdb_file_read(sdb_file_t *file, void *buffer, size_t size) {
  return fread(buffer, size, 1, file);
}

int sdb_file_seek(sdb_file_t *file, off_t offset, int origin) {
  return fseek(file, offset, origin);
}

long sdb_file_size(const char *file_name) {
  sdb_file_t *file = sdb_file_open(file_name);

  if (file == NULL) {
    return 0;
  }

  long position = ftell(file);
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, position, SEEK_END);

  sdb_file_close(file);

  return size;
}

int sdb_file_truncate(const char *file_name) {
  sdb_file_t *file = sdb_file_open(file_name);

  if (file == NULL) {
    return 0;
  }

  int status = ftruncate(fileno(file), 0);
  sdb_file_close(file);

  return status;
}

