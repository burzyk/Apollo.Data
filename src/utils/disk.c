//
// Created by Pawel Burzynski on 14/04/2017.
//

#include "disk.h"

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

