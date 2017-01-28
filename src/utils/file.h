//
// Created by Pawel Burzynski on 18/01/2017.
//

#ifndef APOLLO_STORAGE_FILE_H
#define APOLLO_STORAGE_FILE_H

#include <string>

namespace apollo {

class File {
 public:
  File(std::string file_name);
  virtual ~File();

  void Write(void *buffer, size_t size);
  size_t Read(void *buffer, size_t size);
  void Flush();
  void Seek(off_t offset, int origin);
  size_t GetSize();
 private:
  FILE *f;
};

}

#endif //APOLLO_STORAGE_FILE_H
