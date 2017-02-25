//
// Created by Pawel Burzynski on 03/02/2017.
//

#include <src/fatal-exception.h>
#include <stdlib.h>
#include "thread.h"

namespace shakadb {

Thread::Thread(std::function<void(void *)> thread_routine, Log *log) {
  this->thread = nullptr;
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
