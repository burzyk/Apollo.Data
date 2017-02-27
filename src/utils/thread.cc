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
// Created by Pawel Burzynski on 03/02/2017.
//

#include <src/fatal-exception.h>
#include <stdlib.h>
#include "thread.h"

namespace shakadb {

Thread::Thread(std::function<void(void *)> thread_routine, Log *log) {
  this->thread_data = nullptr;
  this->log = log;
  this->thread_routine = thread_routine;
}

void Thread::Sleep(int milliseconds) {
  struct timespec ts = {
      .tv_sec = milliseconds / 1000, .tv_nsec = (milliseconds % 1000) * 1000000
  };
  nanosleep(&ts, nullptr);
}

void Thread::Start(void *data) {
  this->thread_data = data;
  pthread_create(&this->thread, NULL, ThreadRoutine, this);
}

void Thread::Join() {
  if (pthread_join(this->thread, NULL) != 0) {
    throw new FatalException("Unable to join on a thread");
  }
}

void *Thread::ThreadRoutine(void *data) {
  Thread *_this = (Thread *)data;

  try {
    _this->thread_routine(_this->thread_data);
  } catch (FatalException ex) {
    _this->log->Fatal("Unhandled exception in thread routine: " + std::string(ex.what()));
    exit(-1);
  }

  return nullptr;
}

}
