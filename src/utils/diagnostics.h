//
// Created by Pawel Burzynski on 14/04/2017.
//

#ifndef SRC_UTILS_DIAGNOSTICS_H_
#define SRC_UTILS_DIAGNOSTICS_H_

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sdb_stopwatch_s {
  struct timespec start;
  struct timespec stop;
} sdb_stopwatch_t;

sdb_stopwatch_t *sdb_stopwatch_start();
float sdb_stopwatch_stop_and_destroy(sdb_stopwatch_t *stopwatch);

void sdb_log_fatal(const char *message);
void sdb_log_info(const char *message);
void sdb_log_debug(const char *message);

#ifdef __cplusplus
}
#endif

#endif  // SRC_UTILS_DIAGNOSTICS_H_
