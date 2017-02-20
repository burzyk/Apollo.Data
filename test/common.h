//
// Created by Pawel Burzynski on 03/02/2017.
//

#ifndef SHAKADB_STORAGE_TESTS_COMMON_H
#define SHAKADB_STORAGE_TESTS_COMMON_H

#include <src/log.h>

namespace shakadb {
namespace test {

class NullLog : public Log {
 public:
  virtual void Fatal(std::string message) {};
  virtual void Info(std::string message) {};
  virtual void Debug(std::string message) {};
};

}
}

#endif //SHAKADB_STORAGE_TESTS_COMMON_H
