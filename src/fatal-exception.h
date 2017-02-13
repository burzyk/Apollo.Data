//
// Created by Pawel Burzynski on 22/01/2017.
//

#ifndef SHAKADB_STORAGE_FATALEXCEPTION_H
#define SHAKADB_STORAGE_FATALEXCEPTION_H

#include <exception>
#include <string>
namespace shakadb {

class FatalException : public std::exception {
 public:
  FatalException(const char *message);

  virtual const char *what() const noexcept;
 private:
  const char *message;
};

}

#endif //SHAKADB_STORAGE_FATALEXCEPTION_H
