//
// Created by Pawel Burzynski on 16/02/2017.
//

#ifndef SHAKADB_PACKETLOADER_H
#define SHAKADB_PACKETLOADER_H

#include <memory>
#include "data-packet.h"

namespace shakadb {

class PacketLoader {
 public:
  static DataPacket *Load(Stream *stream);
  static DataPacket *Load(uint8_t *raw_packet, int packet_size);
};

}

#endif //SHAKADB_PACKETLOADER_H
