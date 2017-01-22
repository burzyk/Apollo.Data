//
// Created by Pawel Burzynski on 19/01/2017.
//

#include <sys/stat.h>
#include <src/fatal-exception.h>
#include "directory.h"
#include "log.h"

namespace apollo {

void Directory::CreateDirectory(std::string path) {
  if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
    throw FatalException("Unable to create a directory for data series");
  }
}

}