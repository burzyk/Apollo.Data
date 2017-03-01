/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//
// Created by Pawel Burzynski on 19/01/2017.
//

#include "src/utils/stopwatch.h"

#include <cstdint>

namespace shakadb {

uint64_t Stopwatch::GetTimestamp() {
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  return 1000000000 * ts.tv_sec + ts.tv_nsec;
}

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

}  // namespace shakadb
