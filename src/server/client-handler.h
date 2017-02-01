//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef APOLLO_STORAGE_CLIENTHANDLER_H
#define APOLLO_STORAGE_CLIENTHANDLER_H

#include "data-packet.h"

namespace apollo {

class ClientHandler {
 public:
  virtual void ProcessPacket(data_packet_t *packet) = 0;
};

}

#endif //APOLLO_STORAGE_CLIENTHANDLER_H
