//
// Created by Pawel Burzynski on 16/02/2017.
//

#ifndef SHAKADB_PACKETLOADER_H
#define SHAKADB_PACKETLOADER_H

#include "data-packet.h"

namespace shakadb {

class PacketLoader {
 public:
  static DataPacket *Load(Stream *stream);
};

}

#endif //SHAKADB_PACKETLOADER_H
