//
// Created by Pawel Burzynski on 17/01/2017.
//

#include "bootstrapper.h"
#include "fatal-exception.h"

int main(int argc, char *argv[]) {
  try {
    //std::string config_file(argc > 1 ? argv[1] : "");
    shakadb::Bootstrapper::Run("/Users/pburzynski/projects/ShakaDB/server.cfg");
  } catch (shakadb::FatalException ex) {
    fprintf(stderr, "Fatal Exception: %s\n", ex.what());
    return -1;
  }

  return 0;
}