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

#include "src/utils/diagnostics.h"

#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "src/common.h"
#include "src/utils/memory.h"
#include "src/utils/threading.h"

sdb_stopwatch_t *sdb_stopwatch_start() {
  sdb_stopwatch_t *stopwatch = (sdb_stopwatch_t *)sdb_alloc(sizeof(sdb_stopwatch_t));
  clock_gettime(CLOCK_REALTIME, &stopwatch->_start);

  return stopwatch;
}

float sdb_stopwatch_stop_and_destroy(sdb_stopwatch_t *stopwatch) {
  clock_gettime(CLOCK_REALTIME, &stopwatch->_stop);

  time_t sec = stopwatch->_stop.tv_sec - stopwatch->_start.tv_sec;
  time_t nsec = stopwatch->_stop.tv_nsec - stopwatch->_start.tv_nsec;
  float elapsed = (1000000000 * sec + nsec) / 1000000000.0f;

  sdb_free(stopwatch);
  return elapsed;
}

typedef struct sdb_log_s {
  int verbose;
} sdb_log_t;

sdb_log_t *g_log = NULL;

void sdb_log_write(const char *level, const char *format, va_list args);

void sdb_log_init(int verbose) {
  g_log = (sdb_log_t *)sdb_alloc(sizeof(sdb_log_t));
  g_log->verbose = verbose;
}

void sdb_log_close() {
  if (g_log == NULL) {
    return;
  }

  sdb_free(g_log);
}

void sdb_log_error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  sdb_log_write("ERROR", format, args);
  va_end(args);
}

void sdb_log_info(const char *format, ...) {
  va_list args;
  va_start(args, format);
  sdb_log_write("INFO", format, args);
  va_end(args);
}

void sdb_log_debug(const char *format, ...) {
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
          "%d/%02d/%02d %02d:%02d:%02d.%03lu [%08X] [%s]: %s\n",
          now.tm_year + 1900,
          now.tm_mon + 1,
          now.tm_mday,
          now.tm_hour,
          now.tm_min,
          now.tm_sec,
          tick.tv_nsec / 1000000,
          sdb_thread_get_current_id(),
          level,
          line);

  fflush(stderr);
}
