//
// Created by Pawel Burzynski on 03/02/2017.
//

#ifndef APOLLO_STORAGE_CLIENTRESPONDER_H
#define APOLLO_STORAGE_CLIENTRESPONDER_H

#include "data-packet.h"

namespace apollo {

class ClientResponder {
 public:
  virtual ~ClientResponder() {};
  virtual void SendPacket(int client_id, data_packet_t *packet);
};

}

#endif //APOLLO_STORAGE_CLIENTRESPONDER_H
