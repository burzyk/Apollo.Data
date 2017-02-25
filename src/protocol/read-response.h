//
// Created by Pawel Burzynski on 19/02/2017.
//

#ifndef SHAKADB_READRESPONSE_H
#define SHAKADB_READRESPONSE_H

#include <src/data-point.h>
#include <src/utils/common.h>
#include <src/data-points-reader.h>
#include <src/utils/shallow-buffer.h>
#include "data-packet.h"

namespace shakadb {

class ReadResponse : public DataPacket {
 public:
  ReadResponse();
  ReadResponse(std::shared_ptr<DataPointsReader> reader);

  PacketType GetType();
  int GetPointsCount();
  data_point_t *GetPoints();
 protected:
  bool Deserialize(Buffer *payload);
  std::vector<Buffer *> Serialize();
 private:
  data_point_t *points;
  int points_count;
  std::shared_ptr<DataPointsReader> reader;
};

}

#endif //SHAKADB_READRESPONSE_H
