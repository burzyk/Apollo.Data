//
// Created by Pawel Burzynski on 20/02/2017.
//

#ifndef SHAKADB_CLIENTDATAPOINTREADER_H
#define SHAKADB_CLIENTDATAPOINTREADER_H

#include <src/data-points-reader.h>
#include <functional>
#include <src/protocol/read-response.h>

namespace shakadb {

class ClientDataPointsReader : public DataPointsReader {
 public:
  ClientDataPointsReader(std::function<ReadResponse *(void)> packet_provider);
  virtual ~ClientDataPointsReader();

  data_point_t *GetDataPoints();
  int GetDataPointsCount();
 private:
  data_point_t *received_points;
  int points_count;
  int position;
};

}

#endif //SHAKADB_CLIENTDATAPOINTREADER_H
