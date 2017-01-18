//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cstdlib>

#include "log.h"

namespace apollo {

void Log::Fatal(std::string message) {
  fprintf(stderr, "FATAL: %s\n", message.c_str());
  exit(-1);
}

}