//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef APOLLO_STORAGE_CLIENTHANDLER_H
#define APOLLO_STORAGE_CLIENTHANDLER_H

#include "data-packet.h"
#include "client-responder.h"

namespace apollo {

class ClientHandler {
 public:
  virtual ~ClientHandler() {};
  virtual void ProcessPacket(data_packet_t *packet, ClientResponder *responder) = 0;
};

}

#endif //APOLLO_STORAGE_CLIENTHANDLER_H
