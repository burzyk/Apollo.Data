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

#ifndef SRC_BOOTSTRAPPER_H_
#define SRC_BOOTSTRAPPER_H_

#include <memory>
#include <string>

#include "src/server/server.h"
#include "src/utils/thread.h"
#include "src/middleware/write-handler.h"
#include "src/middleware/read-handler.h"
#include "src/configuration.h"

namespace shakadb {

class Bootstrapper {
 public:
  virtual ~Bootstrapper();
  static Bootstrapper *Run(std::string config_file);
  void Stop();
 private:
  explicit Bootstrapper(Configuration *config);

  void ServerRoutine();
  void MainRoutine();

  Monitor control_lock;
  std::shared_ptr<MonitorScope> control_lock_scope;
  Thread *master_thread;
  Thread *server_thread;

  Log *log;
  Server *server;
  WriteHandler *write_handler;
  ReadHandler *read_handler;
  Database *db;
};

}  // namespace shakadb

#endif  // SRC_BOOTSTRAPPER_H_
