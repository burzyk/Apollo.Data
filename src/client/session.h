//
// Created by Pawel Burzynski on 08/02/2017.
//

#ifndef SHAKADB_STORAGE_SESSION_H
#define SHAKADB_STORAGE_SESSION_H

#include <src/data-point.h>
#include <string>
#include <src/server/data-packet.h>

namespace shakadb {

class Session {
 public:
  static const int kInvalidSocket = -1;

  ~Session();
  static Session *Open(std::string server, int port);

  bool Ping();
 private:
  Session(int sock);
  void SendPacket(PacketType type, uint8_t *data, int size);
  data_packet_t *ReadPacket();
  int sock;
};

}

#endif //SHAKADB_STORAGE_SESSION_H
