//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef APOLLO_STORAGE_DATA_PACKET_H
#define APOLLO_STORAGE_DATA_PACKET_H

#include <cstdint>

namespace apollo {

enum PacketType {
  kPing = 1
};

struct data_packet_t {
  int total_length;
  PacketType type;
  uint8_t data[];
};

}

#endif //APOLLO_STORAGE_DATA_PACKET_H
