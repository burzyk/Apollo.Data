//
// Created by Pawel Burzynski on 15/02/2017.
//

#ifndef SHAKADB_DATABASE_H
#define SHAKADB_DATABASE_H

#include "src/data-points-reader.h"

namespace shakadb {

class Database {
 public:
  virtual ~Database() {};

  virtual std::shared_ptr<DataPointsReader> Read(std::string name, timestamp_t begin, timestamp_t end) = 0;
  virtual void Write(std::string name, data_point_t *points, int count) = 0;
};

}

#endif //SHAKADB_DATABASE_H
