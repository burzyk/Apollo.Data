//
// Created by Pawel Burzynski on 19/01/2017.
//

#include "validation-exception.h"

namespace apollo {
namespace test {

ValidationException::ValidationException(const char *message) {
  this->message = message;
}
const char *ValidationException::what() const noexcept {
  return this->message;
}

}
}

