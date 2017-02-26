//
// Created by Pawel Burzynski on 19/01/2017.
//

#ifndef SHAKADB_STORAGE_STOPWATCH_H
#define SHAKADB_STORAGE_STOPWATCH_H

#include <time.h>
#include <cstdint>

namespace shakadb {

class Stopwatch {
 public:
  static uint64_t GetTimestamp();

  void Start();
  void Stop();
  float GetElapsedSeconds();
 private:
  timespec start;
  timespec stop;
};

}

#endif //SHAKADB_STORAGE_STOPWATCH_H
