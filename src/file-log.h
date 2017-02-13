//
// Created by Pawel Burzynski on 22/01/2017.
//

#ifndef SHAKADB_STORAGE_FILELOG_H
#define SHAKADB_STORAGE_FILELOG_H

#include "log.h"
#include "src/utils/rw-lock.h"

namespace shakadb {

class FileLog : public Log {
 public:
  FileLog(std::string log_file_name);
  ~FileLog();

  void Fatal(std::string message);
  void Info(std::string message);
  void Debug(std::string message);
 private:
  void ToLog(std::string level, std::string message);

  RwLock lock;
  std::string log_file_name;
  FILE *output;
};

}

#endif //SHAKADB_STORAGE_FILELOG_H
