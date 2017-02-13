//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef SHAKADB_STORAGE_DIRECTORY_H
#define SHAKADB_STORAGE_DIRECTORY_H

#include <string>
#include <list>

namespace shakadb {

class Directory {
 public:
  static std::list<std::string> GetDirectories(std::string path);
  static std::list<std::string> GetFiles(std::string path);
  static void CreateDirectory(std::string path);
 private:
  static std::list<std::string> GetDiskItems(std::string path, unsigned char type);
};

}

#endif //SHAKADB_STORAGE_DIRECTORY_H
