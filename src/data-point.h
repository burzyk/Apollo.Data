//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef SHAKADB_STORAGE_DATA_POINT_H
#define SHAKADB_STORAGE_DATA_POINT_H

#include <cstdint>

namespace shakadb {

typedef uint64_t timestamp_t;

struct data_point_t {
  static const timestamp_t kMaxTimestamp;
  static const timestamp_t kMinTimestamp;

  timestamp_t time;
  float value;
};

}

#endif //SHAKADB_STORAGE_DATA_POINT_H
