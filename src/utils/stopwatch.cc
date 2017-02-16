//
// Created by Pawel Burzynski on 19/01/2017.
//

#include <cstdint>
#include "stopwatch.h"

namespace shakadb {

void Stopwatch::Start() {
  clock_gettime(CLOCK_REALTIME, &this->start);
}

void Stopwatch::Stop() {
  clock_gettime(CLOCK_REALTIME, &this->stop);
}

float Stopwatch::GetElapsedSeconds() {
  uint64_t sec = this->stop.tv_sec - this->start.tv_sec;
  uint64_t nsec = this->stop.tv_nsec - this->start.tv_nsec;
  return (1000000000 * sec + nsec) / 1000000000.0;

}

}