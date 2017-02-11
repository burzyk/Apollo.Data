//
// Created by Pawel Burzynski on 11/02/2017.
//

#ifndef APOLLO_STORAGE_ALLOCATOR_H
#define APOLLO_STORAGE_ALLOCATOR_H

#include <string>
#include <map>
#include <cstdlib>
#include <src/fatal-exception.h>
#include "log.h"
#include "rw-lock-scope.h"
#include "rw-lock.h"

namespace apollo {

class Allocator {
 public:
  template<typename T>
  static T *New(int count = 1, const char *context = nullptr) {
    auto scope = lock.LockWrite();

    T *data = (T *)calloc(count, sizeof(T));
    memory[data] = std::string(context != nullptr ? context : "");

    return data;
  }

  static void Delete(void *pointer) {
    auto scope = lock.LockWrite();

    if (memory.find(pointer) == memory.end()) {
      throw FatalException("Deleting unknown pointer");
    }

    memory.erase(pointer);
    free(pointer);
  }

  static void PrintUsage(Log *log) {
    auto scope = lock.LockWrite();
    log->Info("Removing tracking allocator");

    for (auto i : memory) {
      log->Info("Memory was not deleted " + i.second);
    }
  }

 private:
  static RwLock lock;
  static std::map<void *, std::string> memory;
};

}

#endif //APOLLO_STORAGE_ALLOCATOR_H
