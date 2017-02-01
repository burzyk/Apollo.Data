//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef APOLLO_STORAGE_DATA_PACKET_H
#define APOLLO_STORAGE_DATA_PACKET_H

#include <cstdint>

namespace apollo {

typedef int packet_type_t;

struct data_packet_t {
  packet_type_t type;
  int length;
  uint8_t data[];
};

}

#endif //APOLLO_STORAGE_DATA_PACKET_H
