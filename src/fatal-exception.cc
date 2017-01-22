//
// Created by Pawel Burzynski on 22/01/2017.
//

#include "fatal-exception.h"

namespace apollo {

FatalException::FatalException(const char *message) {
  this->message = message;
}
const char *FatalException::what() const noexcept {
  return this->message;
}

}