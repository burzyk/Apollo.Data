//
// Created by Pawel Burzynski on 23/02/2017.
//

#ifndef SHAKADB_PACKETFRAGMENT_H
#define SHAKADB_PACKETFRAGMENT_H

#include <src/utils/common.h>

namespace shakadb {

class PacketFragment {
 public:
  byte_t *GetData();
  int GetSize();
};

}

#endif //SHAKADB_PACKETFRAGMENT_H
