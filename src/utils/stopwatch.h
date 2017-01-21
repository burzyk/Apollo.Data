//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef APOLLO_STORAGE_STOPWATCH_H
#define APOLLO_STORAGE_STOPWATCH_H

#include <time.h>

namespace apollo {

class Stopwatch {
 public:
  void Start();
  void Stop();

  float GetElapsedMilliseconds();
 private:
  timespec start;
  timespec stop;
};

}

#endif //APOLLO_STORAGE_STOPWATCH_H
