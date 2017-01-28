//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef APOLLO_STORAGE_DIRECTORY_H
#define APOLLO_STORAGE_DIRECTORY_H

#include <string>
#include <list>

namespace apollo {

class Directory {
 public:
  static std::list<std::string> GetDirectories(std::string path);
  static std::list<std::string> GetFiles(std::string path);
  static void CreateDirectory(std::string path);
 private:
  static std::list<std::string> GetDiskItems(std::string path, unsigned char type);
};

}

#endif //APOLLO_STORAGE_DIRECTORY_H
