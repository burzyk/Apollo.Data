//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_DATA_POINT_H
#define APOLLO_STORAGE_DATA_POINT_H

#include <cstdint>

namespace apollo {

#define A_MAX_TIMESTAMP     UINT64_MAX
#define A_MIN_TIMESTAMP     0

typedef uint64_t timestamp_t;

struct data_point_t {
  timestamp_t time;
  float value;
};

}

#endif //APOLLO_STORAGE_DATA_POINT_H
