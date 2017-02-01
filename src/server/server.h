//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef APOLLO_STORAGE_SERVER_H
#define APOLLO_STORAGE_SERVER_H

#include <vector>
#include <src/utils/log.h>
#include <list>
#include "client-handler.h"

namespace apollo {

class Server {
 public :
  virtual ~Server() {};
  virtual void Listen() = 0;
};

}

#endif //APOLLO_STORAGE_SERVER_H
