//
// Created by Pawel Burzynski on 22/02/2018.
//

#ifndef SRC_DATA_POINT_H_
#define SRC_DATA_POINT_H_

#include <stdint.h>

typedef uint64_t timestamp_t;

#define SDB_TIMESTAMP_MIN ((timestamp_t)0)
#define SDB_TIMESTAMP_MAX ((timestamp_t)UINT64_MAX)

typedef struct data_point_s {
  timestamp_t time;
  float value;
} __attribute__((packed)) data_point_t;

typedef struct points_list_s {
  data_point_t *points;
  uint64_t count;
  uint32_t value_size;
} points_list_t;

uint64_t data_point_merge(data_point_t *src,
                          uint64_t src_size,
                          data_point_t *dst,
                          uint64_t dst_size,
                          data_point_t **result);
void data_point_sort(data_point_t *points, uint64_t count);
uint64_t data_point_non_zero_distinct(data_point_t *points, uint64_t count);
data_point_t *data_point_find(points_list_t *list, timestamp_t timestamp);
data_point_t *data_point_at(points_list_t *list, uint64_t offset);

#endif  // SRC_DATA_POINT_H_
