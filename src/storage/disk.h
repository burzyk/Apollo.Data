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

#ifndef SRC_STORAGE_DISK_H_
#define SRC_STORAGE_DISK_H_

#include <stdio.h>

typedef FILE sdb_file_t;

int sdb_directory_create(const char *directory_name);

sdb_file_t *sdb_file_open(const char *file_name);
void sdb_file_close(sdb_file_t *file);
size_t sdb_file_write(sdb_file_t *file, void *buffer, size_t size);
size_t sdb_file_read(sdb_file_t *file, void *buffer, size_t size);
int sdb_file_seek(sdb_file_t *file, off_t offset, int origin);
long sdb_file_size(const char *file_name);
int sdb_file_truncate(const char *file_name);

#endif  // SRC_STORAGE_DISK_H_
