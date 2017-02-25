//
// Created by Pawel Burzynski on 15/02/2017.
//

#ifndef SHAKADB_WRITEHANDLER_H
#define SHAKADB_WRITEHANDLER_H

#include <src/server/server.h>
#include <src/database.h>
#include <src/utils/monitor.h>
#include <map>
#include "base-handler.h"

namespace shakadb {

class WriteHandler : public BaseHandler {
 public:
  WriteHandler(Database *db, Server *server);

  void OnPacketReceived(int client_id, DataPacket *packet);
 private:
  Database *db;
};

}

#endif //SHAKADB_WRITEHANDLER_H
