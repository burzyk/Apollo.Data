//
// Created by Pawel Burzynski on 03/02/2017.
//

#ifndef SHAKADB_STORAGE_THREAD_H
#define SHAKADB_STORAGE_THREAD_H

#include <functional>
#include <pthread.h>
#include "log.h"

namespace shakadb {

class Thread {
 public:
  Thread(std::function<void(void *)> thread_routine, Log *log);

  void Start(void *data);
  void Join();
 private:
  static void *ThreadRoutine(void *data);

  Log *log;
  std::function<void(void *)> thread_routine;
  pthread_t thread;
  void *thread_data;
};

}

#endif //SHAKADB_STORAGE_THREAD_H
