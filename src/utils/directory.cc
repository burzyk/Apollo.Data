//
// Created by Pawel Burzynski on 19/01/2017.
//

#include <sys/stat.h>
#include <src/fatal-exception.h>
#include <sys/dirent.h>
#include <dirent.h>
#include "directory.h"
#include "log.h"

namespace apollo {

std::list<std::string> Directory::GetDirectories(std::string path) {
  return GetDiskItems(path, DT_DIR);
}

std::list<std::string> Directory::GetFiles(std::string path) {
  return GetDiskItems(path, DT_REG);
}

void Directory::CreateDirectory(std::string path) {
  if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
    throw FatalException("Unable to create a directory");
  }
}

std::list<std::string> Directory::GetDiskItems(std::string path, unsigned char type) {
  std::list<std::string> result;
  struct dirent *ep = NULL;
  DIR *dir = opendir(path.c_str());

  if (dir == NULL) {
    throw FatalException("Unable to open a directory");
  }

  while ((ep = readdir(dir)) != NULL) {
    if (ep->d_type != type) {
      continue;
    }

    std::string item_name = std::string(ep->d_name);

    if (item_name == "." || item_name == "..") {
      continue;
    }

    result.push_back(item_name);
  }

  closedir(dir);
  return result;
}

}