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

#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#include <stdio.h>
#include <stdint.h>

typedef uint32_t series_id_t;

#ifndef SDB_POINTS_PER_PACKET_MAX
#define SDB_POINTS_PER_PACKET_MAX  6553600
#endif

#ifndef SDB_FILE_MAX_LEN
#define SDB_FILE_MAX_LEN  1024
#endif

#ifndef SDB_DATA_SERIES_MAX
#define SDB_DATA_SERIES_MAX  (1<<20)
#endif

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct buffer_s {
  void *content;
  size_t size;
} buffer_t;

void *sdb_alloc(size_t size);
void sdb_free(void *buffer);

void die_internal(const char *message, const char *file, int line_number);
void assert_internal(int status, const char *message, const char *file, int line_number);

uint64_t sdb_minl(uint64_t a, uint64_t b);
int sdb_min(int a, int b);
uint64_t sdb_maxl(uint64_t a, uint64_t b);
int sdb_max(int a, int b);

#define sdb_assert(status, message) assert_internal(status, message, __FILE__, __LINE__)
#define die(message) die_internal(message, __FILE__, __LINE__)

#endif  // SRC_COMMON_H_
