//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef SHAKADB_STORAGE_DATA_POINT_H
#define SHAKADB_STORAGE_DATA_POINT_H

#include <cstdint>

namespace shakadb {

#define A_MAX_TIMESTAMP     UINT64_MAX
#define A_MIN_TIMESTAMP     0

typedef uint64_t timestamp_t;

struct data_point_t {
  timestamp_t time;
  float value;
};

}

#endif //SHAKADB_STORAGE_DATA_POINT_H
