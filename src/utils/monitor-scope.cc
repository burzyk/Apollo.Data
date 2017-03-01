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
// Created by Pawel Burzynski on 14/02/2017.
//

#include "src/utils/monitor-scope.h"

namespace shakadb {

MonitorScope::MonitorScope(pthread_mutex_t *mutex, pthread_cond_t *cond) {
  this->mutex = mutex;
  this->cond = cond;
}

MonitorScope::~MonitorScope() {
  this->Exit();
}

void MonitorScope::Reenter() {
  pthread_mutex_lock(this->mutex);
}

void MonitorScope::Signal() {
  pthread_cond_signal(this->cond);
}

void MonitorScope::Wait() {
  pthread_cond_wait(this->cond, this->mutex);
}

void MonitorScope::Exit() {
  pthread_mutex_unlock(this->mutex);
}

}  // namespace shakadb
