//
// Created by Pawel Burzynski on 20/02/2017.
//

#ifndef SHAKADB_CLIENTDATAPOINTREADER_H
#define SHAKADB_CLIENTDATAPOINTREADER_H

#include <src/storage/data-point-reader.h>
#include <functional>
#include <src/protocol/read-response.h>

namespace shakadb {

class ClientDataPointReader : public DataPointReader {
 public:
  ClientDataPointReader(std::function<ReadResponse *(void)> packet_provider);
  virtual ~ClientDataPointReader();

  data_point_t *GetDataPoints();
  int GetDataPointsCount();
 private:
  data_point_t *points;
  int points_count;
};

}

#endif //SHAKADB_CLIENTDATAPOINTREADER_H
