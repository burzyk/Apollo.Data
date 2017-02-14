//
// Created by Pawel Burzynski on 14/02/2017.
//

#ifndef SHAKADB_MONITOR_H
#define SHAKADB_MONITOR_H

#include <memory>
#include <pthread.h>
#include "monitor-scope.h"

namespace shakadb {

class Monitor {
 public:
  Monitor();
  ~Monitor();

  std::shared_ptr<MonitorScope> Enter();
 private:
  pthread_mutex_t mutex;
  pthread_cond_t cond;
};

}

#endif //SHAKADB_MONITOR_H
