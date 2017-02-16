//
// Created by Pawel Burzynski on 16/02/2017.
//

#ifndef SHAKADB_PINGPACKET_H
#define SHAKADB_PINGPACKET_H

#include "data-packet.h"

namespace shakadb {

class PingPacket : public DataPacket {
 public:
  PingPacket(Stream *stream, int packet_size);
  PingPacket(uint8_t *ping_data, int ping_data_size);

  virtual PacketType GetType();
  uint8_t *GetPingData();
  int GetPingDataSize();
};

}

#endif //SHAKADB_PINGPACKET_H
