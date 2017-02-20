//
// Created by Pawel Burzynski on 16/02/2017.
//

#ifndef SHAKADB_PINGPACKET_H
#define SHAKADB_PINGPACKET_H

#include "data-packet.h"

namespace shakadb {

class PingPacket : public DataPacket {
 public:
  PingPacket(byte_t *raw_packet, int packet_size);
  PingPacket(char *ping_data, int ping_data_size);

  virtual PacketType GetType();
  char *GetPingData();
  int GetPingDataSize();
};

}

#endif //SHAKADB_PINGPACKET_H
