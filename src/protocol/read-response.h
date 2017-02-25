//
// Created by Pawel Burzynski on 19/02/2017.
//

#ifndef SHAKADB_READRESPONSE_H
#define SHAKADB_READRESPONSE_H

#include <src/data-point.h>
#include <src/utils/common.h>
#include <src/storage/data-points-reader.h>
#include <src/utils/shallow-buffer.h>
#include "data-packet.h"

namespace shakadb {

class ReadResponse : public DataPacket {
 public:
  ReadResponse();
  ReadResponse(DataPointsReader *reader, int front_discard);

  PacketType GetType();
  int GetPointsCount();
  data_point_t *GetPoints();
 protected:
  bool Deserialize(Buffer *payload);
  std::vector<Buffer *> Serialize();
 private:
  data_point_t *points;
  int points_count;
  DataPointsReader *reader;
};

}

#endif //SHAKADB_READRESPONSE_H
