//
// Created by Pawel Burzynski on 03/02/2017.
//

#ifndef APOLLO_STORAGE_COMMON_H
#define APOLLO_STORAGE_COMMON_H

#include <src/utils/log.h>

namespace apollo {
namespace test {

class NullLog : public Log {
 public:
  virtual void Fatal(std::string message) {};
  virtual void Info(std::string message) {};
  virtual void Debug(std::string message) {};
};

}
}

#endif //APOLLO_STORAGE_COMMON_H
