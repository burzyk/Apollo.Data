//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef SHAKADB_STORAGE_STOPWATCH_H
#define SHAKADB_STORAGE_STOPWATCH_H

#include <time.h>

namespace shakadb {

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

#endif //SHAKADB_STORAGE_STOPWATCH_H
