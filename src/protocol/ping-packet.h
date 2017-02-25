//
// Created by Pawel Burzynski on 16/02/2017.
//

#ifndef SHAKADB_PINGPACKET_H
#define SHAKADB_PINGPACKET_H

#include "data-packet.h"

namespace shakadb {

class PingPacket : public DataPacket {
 public:
  friend DataPacket *DataPacket::Load(Stream *stream);
  PingPacket(char *ping_data, int ping_data_size);

  virtual PacketType GetType();
  char *GetPingData();
  int GetPingDataSize();
 private:
  PingPacket(Buffer *packet);
};

}

#endif //SHAKADB_PINGPACKET_H
