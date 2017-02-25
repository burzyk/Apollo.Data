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
  WriteRequest();
  WriteRequest(std::string series_name, data_point_t *points, int points_count);

  PacketType GetType();
  int GetPointsCount();
  data_point_t *GetPoints();
  std::string GetSeriesName();
 protected:
  bool Deserialize(Buffer *payload);
  std::vector<Buffer *> Serialize();
 private:
  struct write_request_t {
    char series_name[SHAKADB_SERIES_NAME_MAX_LENGTH + 1];
    int points_count;
  };

  std::string series_name;
  data_point_t *points;
  int points_count;
};

}

#endif //SHAKADB_WRITEREQUEST_H
