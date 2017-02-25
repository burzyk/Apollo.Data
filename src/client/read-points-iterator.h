//
// Created by Pawel Burzynski on 20/02/2017.
//

#ifndef SHAKADB_CLIENTDATAPOINTREADER_H
#define SHAKADB_CLIENTDATAPOINTREADER_H

#include <src/data-points-reader.h>
#include <functional>
#include <src/protocol/read-response.h>

namespace shakadb {

class ReadPointsIterator {
 public:
  ReadPointsIterator(Stream *response);
  ~ReadPointsIterator();

  data_point_t *CurrentDataPoints();
  int CurrentDataPointsCount();
  bool MoveNext();
 private:
  Stream *response;
  ReadResponse *current;
};

}

#endif //SHAKADB_CLIENTDATAPOINTREADER_H
