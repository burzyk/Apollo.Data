//
// Created by Pawel Burzynski on 16/02/2017.
//

#ifndef SHAKADB_WRITEREQUEST_H
#define SHAKADB_WRITEREQUEST_H

#include <src/data-point.h>
#include <string>
#include "data-packet.h"

namespace shakadb {

class WriteRequest : public DataPacket {
 public:
  WriteRequest(RingBuffer *buffer, int packet_size);
  WriteRequest(std::string series_name, data_point_t *points, int points_count);

  PacketType GetType();
  int GetPointsCount();
  data_point_t *GetPoints();
  std::string GetSeriesName();
 private:
  int GetSeriesNameSize();
};

}

#endif //SHAKADB_WRITEREQUEST_H
