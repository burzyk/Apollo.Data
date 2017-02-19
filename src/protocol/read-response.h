//
// Created by Pawel Burzynski on 19/02/2017.
//

#ifndef SHAKADB_READRESPONSE_H
#define SHAKADB_READRESPONSE_H

#include <src/data-point.h>
#include "data-packet.h"

namespace shakadb {

class ReadResponse : public DataPacket {
 public:
  ReadResponse(uint8_t *raw_packet, int packet_size);
  ReadResponse(data_point_t *points, int points_count);

  PacketType GetType();
  int GetPointsCount();
  data_point_t *GetPoints();
};

}

#endif //SHAKADB_READRESPONSE_H
