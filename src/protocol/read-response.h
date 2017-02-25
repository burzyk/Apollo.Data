//
// Created by Pawel Burzynski on 19/02/2017.
//

#ifndef SHAKADB_READRESPONSE_H
#define SHAKADB_READRESPONSE_H

#include <src/data-point.h>
#include <src/utils/common.h>
#include "data-packet.h"

namespace shakadb {

class ReadResponse : public DataPacket {
 public:
  friend DataPacket *DataPacket::Load(Stream *stream);
  ReadResponse(data_point_t *points, int points_count, int total_points_count);

  PacketType GetType();
  int GetTotalPointsCount();
  int GetPointsCount();
  data_point_t *GetPoints();
 private:
  ReadResponse(Buffer *packet);
};

}

#endif //SHAKADB_READRESPONSE_H
