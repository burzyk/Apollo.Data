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

#include "src/common.h"

#include <stdlib.h>

void *sdb_alloc(size_t size) {
  return calloc(1, size);
}

void *sdb_realloc(void *buffer, size_t size) {
  return realloc(buffer, size);
}

void sdb_free(void *buffer) {
  free(buffer);
}

int sdb_data_point_compare(sdb_data_point_t *lhs, sdb_data_point_t *rhs) {
  return lhs->time == rhs->time ? 0 : lhs->time < rhs->time ? -1 : 1;
}

inline uint64_t sdb_minl(uint64_t a, uint64_t b) {
  return a < b ? a : b;
}

inline int sdb_min(int a, int b) {
  return a < b ? a : b;
}
inline uint64_t sdb_maxl(uint64_t a, uint64_t b) {
  return a < b ? b : a;
}

inline int sdb_max(int a, int b) {
  return a < b ? b : a;
}

int sdb_find(void *elements, int element_size, int elements_count, void *data, sdb_find_predicate predicate) {
  if (elements == NULL || element_size == 0 || elements_count == 0 || data == NULL) {
    return -1;
  }

  char *ptr = elements;
  int left = 0;
  int right = elements_count;

  if (predicate(data, ptr) < 0) {
    return 0;
  }

  if (predicate(data, ptr + (elements_count - 1) * element_size) > 0) {
    return elements_count;
  }

  while (left < right) {
    int mid = (right + left) / 2;
    int cmp = predicate(data, ptr + mid * element_size);

    if (cmp < 0) {
      right = mid;
    } else if (cmp > 0) {
      left = mid + 1;
    } else {
      return mid;
    }
  }

  return left;
}

void die_internal(const char *message, const char *file, int line_number) {
  fprintf(stderr, "PANIC: %s:%d -> %s\n", file, line_number, message);
  fflush(stderr);
  exit(-1);
}

void sdb_assert_internal(int status, const char *message, const char *file, int line_number) {
  if (!status) {
    char line[SDB_FILE_MAX_LEN] = {0};
    sprintf(line, "ASSERT: %s:%d -> %s", file, line_number, message);
    die(line);
  }
}
