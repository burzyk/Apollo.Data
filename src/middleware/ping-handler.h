//
// Created by Pawel Burzynski on 13/02/2017.
//

#ifndef SHAKADB_STORAGE_PINGHANDLER_H
#define SHAKADB_STORAGE_PINGHANDLER_H

#include <src/server/server.h>
#include "base-handler.h"

namespace shakadb {

class PingHandler : public BaseHandler {
 public:
  void OnReceived(ServerClient *client, DataPacket *packet);
};

}

#endif //SHAKADB_STORAGE_PINGHANDLER_H
