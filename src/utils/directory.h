//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef APOLLO_STORAGE_DIRECTORY_H
#define APOLLO_STORAGE_DIRECTORY_H

#include <string>

namespace apollo {

class Directory {
 public:
  static void CreateDirectory(std::string path);
};

}

#endif //APOLLO_STORAGE_DIRECTORY_H
