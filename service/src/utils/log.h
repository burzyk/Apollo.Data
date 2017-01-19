//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef SRC_LOG_H
#define SRC_LOG_H

#include <string>

namespace apollo {

class Log {
 public:
  static void Fatal(std::string message);
};

}

#endif //SRC_LOG_H
