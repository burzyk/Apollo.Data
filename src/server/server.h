//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef SHAKADB_STORAGE_SERVER_H
#define SHAKADB_STORAGE_SERVER_H

#include <vector>
#include <src/log.h>
#include <list>
#include "server-client.h"

namespace shakadb {

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

#endif //SHAKADB_STORAGE_SERVER_H
