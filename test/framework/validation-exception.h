//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef APOLLO_STORAGE_VALIDATIONEXCEPTION_H
#define APOLLO_STORAGE_VALIDATIONEXCEPTION_H

#include <exception>

namespace apollo {
namespace test {

class ValidationException : public std::exception {
 public:
  ValidationException(const char *message);

  virtual const char *what() const noexcept;
 private:
  const char *message;
};

}
}

#endif //APOLLO_STORAGE_VALIDATIONEXCEPTION_H
