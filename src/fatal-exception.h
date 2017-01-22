//
// Created by Pawel Burzynski on 22/01/2017.
//

#ifndef APOLLO_STORAGE_FATALEXCEPTION_H
#define APOLLO_STORAGE_FATALEXCEPTION_H

#include <exception>
#include <string>
namespace apollo {

class FatalException : public std::exception {
 public:
  FatalException(const char *message);

  virtual const char *what() const noexcept;
 private:
  const char *message;
};

}

#endif //APOLLO_STORAGE_FATALEXCEPTION_H
