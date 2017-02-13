//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef APOLLO_STORAGE_SERVER_H
#define APOLLO_STORAGE_SERVER_H

#include <vector>
#include <src/utils/log.h>
#include <list>
#include "server-client.h"

namespace apollo {

class Server {
 public:
  class ClientConnectedListener {
   public:
    virtual void OnClientConnected(Server *server, ServerClient *client) = 0;
  };

  virtual ~Server() {};
  virtual void Listen() = 0;
  virtual void Close() = 0;
  virtual void AddClientConnectedListener(ClientConnectedListener *listener) = 0;
};

}

#endif //APOLLO_STORAGE_SERVER_H
