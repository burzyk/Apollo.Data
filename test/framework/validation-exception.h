//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef SHAKADB_STORAGE_VALIDATIONEXCEPTION_H
#define SHAKADB_STORAGE_VALIDATIONEXCEPTION_H

#include <exception>

namespace shakadb {
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

#endif //SHAKADB_STORAGE_VALIDATIONEXCEPTION_H
