//
// Created by Pawel Burzynski on 14/04/2017.
//

#include <stdint.h>
#include "src/utils/diagnostics.h"
#include "src/utils/memory.h"

sdb_stopwatch_t *sdb_stopwatch_start() {
  sdb_stopwatch_t *stopwatch = (sdb_stopwatch_t *)sdb_alloc(sizeof(sdb_stopwatch_t));
  clock_gettime(CLOCK_REALTIME, &stopwatch->start);

  return stopwatch;
}

float sdb_stopwatch_stop_and_destroy(sdb_stopwatch_t *stopwatch) {
  clock_gettime(CLOCK_REALTIME, &stopwatch->stop);

  time_t sec = stopwatch->stop.tv_sec - stopwatch->start.tv_sec;
  time_t nsec = stopwatch->stop.tv_nsec - stopwatch->start.tv_nsec;
  float elapsed = (1000000000 * sec + nsec) / 1000000000.0f;

  sdb_free(stopwatch);
  return elapsed;
}
