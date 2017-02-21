//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <src/utils/allocator.h>
#include "bootstrapper.h"
#include "fatal-exception.h"

int main(int argc, char *argv[]) {
  try {
    //std::string config_file(argc > 1 ? argv[1] : "");
    shakadb::Bootstrapper *bootstrapper = shakadb::Bootstrapper::Run("");

    while (getc(stdin) != 'q');

    bootstrapper->Stop();
    delete bootstrapper;

    shakadb::Allocator::AssertAllDeleted();
  } catch (shakadb::FatalException ex) {
    fprintf(stderr, "Fatal Exception: %s\n", ex.what());
    return -1;
  }

  return 0;
}