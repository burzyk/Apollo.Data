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
  sdb_mutex_t *lock;
  char log_file_name[SDB_FILE_MAX_LEN];
  FILE *output;
} sdb_log_t;

sdb_log_t *g_log = NULL;

void sdb_log_write(const char *level, const char *message);

void sdb_log_init(const char *log_file_name) {
  g_log = (sdb_log_t *)sdb_alloc(sizeof(sdb_log_t));
  strncpy(g_log->log_file_name, log_file_name, SDB_FILE_MAX_LEN);
  g_log->lock = sdb_mutex_create();
  g_log->output = NULL;
}

void sdb_log_close() {
  if (g_log == NULL) {
    return;
  }

  if (g_log->output != stdout) {
    fclose(g_log->output);
  }

  sdb_mutex_destroy(g_log->lock);
  sdb_free(g_log);
}

void sdb_log_fatal(const char *message) {
  sdb_log_write("FATAL", message);
}

void sdb_log_info(const char *message) {
  sdb_log_write("INFO", message);
}

void sdb_log_debug(const char *message) {
  sdb_log_write("DEBUG", message);
}

void sdb_log_write(const char *level, const char *message) {
  if (g_log == NULL) {
    return;
  }

  sdb_mutex_lock(g_log->lock);

  if (g_log->output == NULL) {
    g_log->output = strcmp(g_log->log_file_name, "") == 0
                    ? stdout
                    : fopen(g_log->log_file_name, "a+");

    if (g_log->output == NULL) {
      die("Unable to open log file");
    }
  }

  time_t t = time(NULL);
  struct tm now;
  localtime_r(&t, &now);

  fprintf(g_log->output,
          "%d/%02d/%02d [%s]: %s\n",
          now.tm_year + 1900,
          now.tm_mon + 1,
          now.tm_mday,
          level,
          message);

  if (!strcmp(level, "FATAL")) {
    fflush(g_log->output);
  }

  sdb_mutex_unlock(g_log->lock);
}
