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
// Created by Pawel Burzynski on 20/02/2017.
//

#include <src/utils/file.h>
#include <src/utils/allocator.h>
#include <sstream>
#include <regex.h>
#include "configuration.h"

namespace shakadb {

Configuration *Configuration::Load(std::string config_file) {
  FILE *f = fopen(config_file.c_str(), "r");
  Configuration *config = new Configuration();

  if (f == nullptr) {
    return config;
  }

  char *line = nullptr;
  size_t line_size = 0;
  ssize_t read = 0;
  regex_t regex;

  if (regcomp(&regex, "^.+=.+$", REG_EXTENDED)) {
    throw FatalException("Unable to compile a regex");
  };

  while ((read = getline(&line, &line_size, f)) != -1) {
    if (read == 0 || line[0] == '#') {
      continue;
    }

    if (regexec(&regex, line, 0, NULL, 0) == REG_NOMATCH) {
      continue;
    }

    std::string key;
    std::string value;
    bool key_complete = false;

    for (int i = 0; i < read; i++) {
      if (isspace(line[i])) {
        continue;
      }

      if (line[i] == '=') {
        key_complete = true;
      } else {
        if (!key_complete) {
          key += line[i];
        } else {
          value += line[i];
        }
      }
    }

    config->config[key] = value;
  }

  free(line);
  fclose(f);

  return config;
}

Configuration::Configuration() {
}

int Configuration::ReadAsInt(std::string key, int defaultValue) {
  return this->config.find(key) == this->config.end() ? defaultValue : atoi(this->config[key].c_str());
}

std::string Configuration::ReadAsString(std::string key, std::string defaultValue) {
  return this->config.find(key) == this->config.end() ? defaultValue : this->config[key];
}

std::string Configuration::GetLogFile() {
  std::string value = this->ReadAsString("log.file", "");
  return value == "(stdout)" ? "" : value;
}

int Configuration::GetServerPort() {
  return this->ReadAsInt("server.port", 8099);
}

int Configuration::GetServerBacklog() {
  return this->ReadAsInt("server.backlog", 10);
}

std::string Configuration::GetDbFolder() {
  return this->ReadAsString("db.folder", "/usr/local/shakadb/data");
}

int Configuration::GetDbPointsPerChunk() {
  return this->ReadAsInt("db.points_per_chunk", 100000);
}

}