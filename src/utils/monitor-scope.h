//
// Created by Pawel Burzynski on 14/02/2017.
//

#ifndef SHAKADB_MONITORSCOPE_H
#define SHAKADB_MONITORSCOPE_H

#include <pthread.h>

namespace shakadb {

class MonitorScope {
 public:
  MonitorScope(pthread_mutex_t *mutex, pthread_cond_t *cond);
  ~MonitorScope();

  void Signal();
  void Wait();
  void Exit();
 private:
  pthread_mutex_t *mutex;
  pthread_cond_t *cond;
};

}

#endif //SHAKADB_MONITORSCOPE_H
