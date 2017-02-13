//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef SHAKADB_STORAGE_DATA_PACKET_H
#define SHAKADB_STORAGE_DATA_PACKET_H

#include <cstdint>

namespace shakadb {

enum PacketType {
  kPing = 1
};

struct data_packet_t {
  int total_length;
  PacketType type;
  uint8_t data[];
};

}

#endif //SHAKADB_STORAGE_DATA_PACKET_H
