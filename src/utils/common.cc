//
// Created by Pawel Burzynski on 20/02/2017.
//

#include "common.h"

namespace shakadb {

uint64_t max(uint64_t a, uint64_t b) {
  return a < b ? b : a;
}

uint64_t min(uint64_t a, uint64_t b) {
  return a < b ? a : b;
}

int min(int a, int b) {
  return a < b ? a : b;
}



}

