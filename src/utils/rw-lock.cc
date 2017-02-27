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
// Created by Pawel Burzynski on 22/01/2017.
//

#include <cstdio>
#include "rw-lock.h"

namespace shakadb {

RwLock::RwLock() {
  this->rwlock = PTHREAD_RWLOCK_INITIALIZER;
}

RwLock::~RwLock() {
  pthread_rwlock_unlock(&this->rwlock);
  pthread_rwlock_destroy(&this->rwlock);
}

std::shared_ptr<RwLockScope> RwLock::LockRead() {
  pthread_rwlock_rdlock(&this->rwlock);
  return std::make_shared<RwLockScope>(&this->rwlock);
}

std::shared_ptr<RwLockScope> RwLock::LockWrite() {
  pthread_rwlock_wrlock(&this->rwlock);
  return std::make_shared<RwLockScope>(&this->rwlock);
}

}