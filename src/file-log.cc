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

#include "src/file-log.h"

#include <cstdlib>

#include "src/fatal-exception.h"
#include "c_common.h"

namespace shakadb {

FileLog::FileLog(std::string log_file_name) {
  this->log_file_name = log_file_name;
  this->output = nullptr;
  this->lock = sdb_monitor_create();
}

FileLog::~FileLog() {
  if (this->output != nullptr && this->log_file_name != "") {
    fclose(this->output);
  }

  sdb_monitor_destroy(this->lock);
}

void FileLog::Fatal(std::string message) {
  this->ToLog("FATAL", message);
}

void FileLog::Info(std::string message) {
  ToLog("INFO", message);
}

void FileLog::Debug(std::string message) {
  ToLog("DEBUG", message);
}

void FileLog::ToLog(std::string level, std::string message) {
  sdb_monitor_enter(this->lock);

  if (this->output == nullptr) {
    this->output = this->log_file_name == ""
                   ? stdout
                   : fopen(this->log_file_name.c_str(), "a+");

    if (this->output == nullptr) {
      die("Unable to open log file");
    }
  }

  time_t t = time(nullptr);
  struct tm now;
  localtime_r(&t, &now);

  fprintf(this->output,
          "%d/%02d/%02d [%s]: %s\n",
          now.tm_year + 1900,
          now.tm_mon + 1,
          now.tm_mday,
          level.c_str(),
          message.c_str());

  if (level == "FATAL") {
    fflush(this->output);
  }

  sdb_monitor_exit(this->lock);
}

}  // namespace shakadb
