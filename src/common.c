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