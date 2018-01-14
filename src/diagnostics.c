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

#include "diagnostics.h"

#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "src/common.h"

uint64_t sdb_now();

uint64_t sdb_now() {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  return (uint64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
}

stopwatch_t *stopwatch_start() {
  stopwatch_t *stopwatch = (stopwatch_t *)sdb_alloc(sizeof(stopwatch_t));
  stopwatch->start = sdb_now();

  return stopwatch;
}

float stopwatch_stop_and_destroy(stopwatch_t *stopwatch) {
  stopwatch->stop = sdb_now();

  float elapsed = (stopwatch->stop - stopwatch->start) / 1000000000.0f;

  sdb_free(stopwatch);
  return elapsed;
}

typedef struct sdb_log_s {
  int verbose;
} sdb_log_t;

sdb_log_t *g_log = NULL;

void sdb_log_write(const char *level, const char *format, va_list args);

void log_init(int verbose) {
  g_log = (sdb_log_t *)sdb_alloc(sizeof(sdb_log_t));
  g_log->verbose = verbose;
}

void log_close() {
  if (g_log == NULL) {
    return;
  }

  sdb_free(g_log);
}

void log_error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  sdb_log_write("ERROR", format, args);
  va_end(args);
}

void log_info(const char *format, ...) {
  va_list args;
  va_start(args, format);
  sdb_log_write("INFO", format, args);
  va_end(args);
}

void log_debug(const char *format, ...) {
  if (g_log == NULL || !g_log->verbose) {
    return;
  }

  va_list args;
  va_start(args, format);
  sdb_log_write("DEBUG", format, args);
  va_end(args);
}

void sdb_log_write(const char *level, const char *format, va_list args) {
  if (g_log == NULL) {
    return;
  }

  char line[SDB_LOG_LINE_MAX_LEN] = {0};
  vsnprintf(line, SDB_LOG_LINE_MAX_LEN, format, args);

  struct timespec tick;
  clock_gettime(CLOCK_REALTIME, &tick);
  struct tm now;
  localtime_r(&tick.tv_sec, &now);

  fprintf(stderr,
          "%d/%02d/%02d %02d:%02d:%02d.%03lu [%s]: %s\n",
          now.tm_year + 1900,
          now.tm_mon + 1,
          now.tm_mday,
          now.tm_hour,
          now.tm_min,
          now.tm_sec,
          tick.tv_nsec / 1000000,
          level,
          line);

  fflush(stderr);
}
