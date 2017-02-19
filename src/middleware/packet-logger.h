//
// Created by Pawel Burzynski on 03/02/2017.
//

#ifndef SHAKADB_STORAGE_PACKETLOGGER_H
#define SHAKADB_STORAGE_PACKETLOGGER_H

#include <src/log.h>
#include <src/server/server.h>
#include "base-handler.h"

namespace shakadb {

class PacketLogger : public BaseHandler {
 public:
  PacketLogger(Log *log);

  void OnReceived(ServerClient *client, DataPacket *packet);
 private:
  Log *log;
};

}

#endif //SHAKADB_STORAGE_PACKETLOGGER_H
