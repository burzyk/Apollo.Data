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
  ReadHandler(Database *db, int points_per_packet);

  void OnReceived(ServerClient *client, DataPacket *packet);
 private:
  Database *db;
  int points_per_packet;
};

}

#endif //SHAKADB_READHANDLER_H
