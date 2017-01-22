//
// Created by Pawel Burzynski on 22/01/2017.
//

#include <src/fatal-exception.h>
#include <cstdlib>
#include "file-log.h"

namespace apollo {

FileLog::FileLog(std::string log_file_name) {
  this->log_file_name = log_file_name;
  this->output = nullptr;
}

FileLog::~FileLog() {
  if (this->output != nullptr && this->log_file_name != "") {
    fclose(this->output);
  }
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
  if (this->output == nullptr) {
    this->output = this->log_file_name == ""
                   ? stdout
                   : fopen(this->log_file_name.c_str(), "a+");

    if (this->output == nullptr) {
      fprintf(stderr, "Unable to open log file: %s\n", this->log_file_name.c_str());
      exit(-1);
    }
  }

  time_t t = time(nullptr);
  struct tm *now = localtime(&t);

  fprintf(this->output,
          "%d/%02d/%02d [%s]: %s\n",
          now->tm_year + 1900,
          now->tm_mon + 1,
          now->tm_mday,
          level.c_str(),
          message.c_str());
}

}
