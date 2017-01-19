//
// Created by Pawel Burzynski on 19/01/2017.
//

#include "stopwatch.h"

namespace apollo {

void Stopwatch::Start() {
  clock_gettime(CLOCK_REALTIME, &this->start);
}

void Stopwatch::Stop() {
  clock_gettime(CLOCK_REALTIME, &this->stop);
}

float Stopwatch::GetElapsedMiliseconds() {
  long sec = this->stop.tv_nsec - this->start.tv_nsec;
  long nsec = this->stop.tv_nsec - this->start.tv_nsec;
  return (1000000000 * sec + nsec) / 1000.0f;

}

}