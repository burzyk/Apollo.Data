//
// Created by Pawel Burzynski on 12/02/2017.
//

#ifndef SHAKADB_STORAGE_SERVERCLIENT_H
#define SHAKADB_STORAGE_SERVERCLIENT_H

#include <cstdint>
#include "src/protocol/data-packet.h"

namespace shakadb {

class ServerClient {
 public:
  class ServerClientListener {
   public:
    virtual ~ServerClientListener() {};
    virtual void OnReceived(ServerClient *client, DataPacket *packet) = 0;
    virtual void OnDisconnected(ServerClient *client) = 0;
  };

  virtual ~ServerClient() {};
  virtual void AddServerClientListener(ServerClientListener *listener) = 0;
  virtual void SendPacket(DataPacket *packet) = 0;
  virtual void Close() = 0;
};

}

#endif //SHAKADB_STORAGE_SERVERCLIENT_H