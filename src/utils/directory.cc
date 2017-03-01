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

#include "src/utils/directory.h"

#include <sys/stat.h>
#include <dirent.h>
#include <cerrno>

#include "src/fatal-exception.h"
#include "src/log.h"

namespace shakadb {

std::list<std::string> Directory::GetDirectories(std::string path) {
  return GetDiskItems(path, DT_DIR);
}

std::list<std::string> Directory::GetFiles(std::string path) {
  return GetDiskItems(path, DT_REG);
}

void Directory::CreateDirectory(std::string path) {
  int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  if (status && errno != EEXIST) {
    throw FatalException("Unable to create a directory");
  }
}

std::list<std::string> Directory::GetDiskItems(
    std::string path,
    unsigned char type) {
  std::list<std::string> result;
  struct dirent *ep = NULL;
  DIR *dir = opendir(path.c_str());

  if (dir == NULL) {
    throw FatalException("Unable to open a directory");
  }

  while ((ep = readdir(dir)) != NULL) {
    if (ep->d_type != type) {
      continue;
    }

    std::string item_name = std::string(ep->d_name);

    if (item_name == "." || item_name == "..") {
      continue;
    }

    result.push_back(item_name);
  }

  closedir(dir);
  return result;
}

}  // namespace shakadb
