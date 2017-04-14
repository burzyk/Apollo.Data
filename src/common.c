//
// Created by Pawel Burzynski on 14/04/2017.
//

#include "src/common.h"

#include <stdio.h>
#include <stdlib.h>

void die(const char *message) {
  fprintf(stderr, "%s\n", message);
  exit(-1);
}

sdb_timestamp_t sdb_min(sdb_timestamp_t a, sdb_timestamp_t b) {
  return a < b ? a : b;
}

sdb_timestamp_t sdb_max(sdb_timestamp_t a, sdb_timestamp_t b) {
  return a < b ? b : a;
}