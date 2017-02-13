//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef SRC_LOG_H
#define SRC_LOG_H

#include <string>

namespace shakadb {

class Log {
 public:
  virtual ~Log() {};
  virtual void Fatal(std::string message) = 0;
  virtual void Info(std::string message) = 0;
  virtual void Debug(std::string message) = 0;
};

}

#endif //SRC_LOG_H
