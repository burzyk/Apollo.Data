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
  uint8_t value[];
} __attribute__((packed)) data_point_t;

typedef struct points_list_s {
  data_point_t *content;
  uint64_t count;
  uint32_t point_size;
} points_list_t;

void data_point_merge(points_list_t *src, points_list_t *dst, points_list_t *result);
void data_point_sort(points_list_t *points);
uint64_t data_point_non_zero_distinct(points_list_t *points);
data_point_t *data_point_find(points_list_t *points, timestamp_t timestamp);
data_point_t *data_point_at(points_list_t *points, uint64_t offset);
data_point_t *data_point_next(points_list_t *points, data_point_t *curr);
data_point_t *data_point_prev(points_list_t *points, data_point_t *curr);
data_point_t *points_list_end(points_list_t *points);
data_point_t *points_list_last(points_list_t *points);
uint64_t data_point_index(points_list_t *points, data_point_t *point);
uint64_t data_point_dist(points_list_t *points, data_point_t *start, data_point_t *end);

#endif  // SRC_DATA_POINT_H_
