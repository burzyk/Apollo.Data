//
// Created by Pawel Burzynski on 14/04/2017.
//

#ifndef SRC_UTILS_DISK_H_
#define SRC_UTILS_DISK_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef FILE sdb_file_t;

int sdb_directory_create(const char *directory_name);

sdb_file_t *sdb_file_open(const char *file_name);
void sdb_file_close(sdb_file_t *file);
size_t sdb_file_write(sdb_file_t *file, void *buffer, size_t size);
size_t sdb_file_read(sdb_file_t *file, void *buffer, size_t size);
int sdb_file_seek(sdb_file_t *file, off_t offset, int origin);
long sdb_file_size(const char *file_name);
int sdb_file_truncate(const char *file_name);

#ifdef __cplusplus
};
#endif

#endif  // SRC_UTILS_DISK_H_
