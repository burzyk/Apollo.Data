//
// Created by Pawel Burzynski on 17/01/2017.
//

#include "bootstrapper.h"
#include "fatal-exception.h"

int main() {
  try {
    shakadb::Bootstrapper::Run();
  } catch (shakadb::FatalException ex) {
    fprintf(stderr, "Fatal Exception: %s\n", ex.what());
    return -1;
  }

  return 0;
}