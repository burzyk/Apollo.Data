//
// Created by Pawel Burzynski on 03/02/2017.
//

#ifndef APOLLO_STORAGE_PACKETLOGGER_H
#define APOLLO_STORAGE_PACKETLOGGER_H

#include <src/server/client-handler.h>
#include <src/utils/log.h>

namespace apollo {

class PacketLogger : public ClientHandler {
 public:
  PacketLogger(Log *log);
  void ProcessPacket(data_packet_t *packet, ClientResponder *responder);
 private:
  Log *log;
};

}

#endif //APOLLO_STORAGE_PACKETLOGGER_H
