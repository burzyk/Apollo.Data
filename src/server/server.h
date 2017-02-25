//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef SHAKADB_STORAGE_SERVER_H
#define SHAKADB_STORAGE_SERVER_H

#include <vector>
#include <src/log.h>
#include <list>
#include <src/protocol/data-packet.h>

namespace shakadb {

class Server {
 public:
  class ServerListener {
   public:
    virtual ~ServerListener() {};
    virtual void OnClientConnected(int client_id) = 0;
    virtual void OnClientDisconnected(int client_id) = 0;
    virtual void OnPacketReceived(int client_id, DataPacket *packet) = 0;
  };

  virtual ~Server() {};
  virtual void Listen() = 0;
  virtual void Close() = 0;
  virtual void AddServerListener(ServerListener *listener) = 0;
  virtual void SendPacket(int client_id, DataPacket *packet) = 0;
};

}

#endif //SHAKADB_STORAGE_SERVER_H
