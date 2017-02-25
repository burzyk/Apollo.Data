//
// Created by Pawel Burzynski on 19/02/2017.
//

#ifndef SHAKADB_READHANDLER_H
#define SHAKADB_READHANDLER_H

#include <src/database.h>
#include "base-handler.h"

namespace shakadb {

class ReadHandler : public BaseHandler {
 public:
  ReadHandler(Database *db, Server *server, int points_per_packet);

  void OnPacketReceived(int client_id, DataPacket *packet);
 private:
  Database *db;
  int points_per_packet;
};

}

#endif //SHAKADB_READHANDLER_H
