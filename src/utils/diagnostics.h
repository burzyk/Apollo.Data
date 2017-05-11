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

#ifndef SRC_UTILS_DIAGNOSTICS_H_
#define SRC_UTILS_DIAGNOSTICS_H_

#include <time.h>

#define SDB_LOG_LINE_MAX_LEN  1024

typedef struct sdb_stopwatch_s {
  struct timespec _start;
  struct timespec _stop;
} sdb_stopwatch_t;

sdb_stopwatch_t *sdb_stopwatch_start();
float sdb_stopwatch_stop_and_destroy(sdb_stopwatch_t *stopwatch);

void sdb_log_init(const char *log_file_name, int verbose);
void sdb_log_close();
void sdb_log_error(const char *format, ...);
void sdb_log_info(const char *format, ...);
void sdb_log_debug(const char *format, ...);

#endif  // SRC_UTILS_DIAGNOSTICS_H_
