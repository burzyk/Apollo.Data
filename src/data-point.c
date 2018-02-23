//
// Created by Pawel Burzynski on 22/02/2018.
//

#include <stdlib.h>
#include "src/data-point.h"
#include "src/common.h"

int data_point_compare(data_point_t *lhs, data_point_t *rhs);

inline int data_point_compare(data_point_t *lhs, data_point_t *rhs) {
  return lhs->time == rhs->time ? 0 : lhs->time < rhs->time ? -1 : 1;
}

uint64_t data_point_merge(data_point_t *src,
                          uint64_t src_size,
                          data_point_t *dst,
                          uint64_t dst_size,
                          data_point_t **result) {
  uint64_t buffer_count = src_size + dst_size;
  data_point_t *buffer = (data_point_t *)sdb_alloc(buffer_count * sizeof(data_point_t));
  uint64_t src_pos = 0;
  uint64_t dst_pos = 0;
  uint64_t duplicated_count = 0;

  for (uint64_t i = 0; i < buffer_count - duplicated_count; i++) {
    if (src_pos == src_size) {
      buffer[i] = dst[dst_pos++];
    } else if (dst_pos == dst_size) {
      buffer[i] = src[src_pos++];
    } else if (dst[dst_pos].time < src[src_pos].time) {
      buffer[i] = dst[dst_pos++];
    } else if (src[src_pos].time < dst[dst_pos].time) {
      buffer[i] = src[src_pos++];
    } else {
      buffer[i] = src[src_pos++];
      dst_pos++;
      duplicated_count++;
    }
  }

  *result = buffer;
  return buffer_count - duplicated_count;
}

void data_point_sort(data_point_t *points, uint64_t count) {
  qsort(points, (size_t)count, sizeof(data_point_t), (int (*)(const void *, const void *))data_point_compare);
}

uint64_t data_point_non_zero_distinct(data_point_t *points, uint64_t count) {
  int64_t tail = -1;

  for (uint64_t i = 0; i < count; i++) {
    if (points[i].time == 0) {
      continue;
    }

    if (tail < 0 || points[tail].time < points[i].time) {
      tail++;
    }

    points[tail] = points[i];
  }

  tail++;

  return (uint64_t)tail;
}

inline data_point_t *data_point_at(points_list_t *list, uint64_t offset) {
  return (data_point_t *)(((uint8_t *)list->points) + offset * (list->value_size + sizeof(timestamp_t)));
}

data_point_t *data_point_find(points_list_t *list, timestamp_t timestamp) {
  if (list->points == NULL || list->count == 0) {
    return NULL;
  }

  uint64_t left = 0;
  uint64_t right = list->count;
  data_point_t element = {.time = timestamp};

  if (data_point_compare(&element, list->points) < 0) {
    return list->points;
  }

  if (data_point_compare(&element, data_point_at(list, list->count - 1)) > 0) {
    return data_point_at(list, list->count);
  }

  while (left < right) {
    uint64_t mid = (right + left) / 2;
    int cmp = data_point_compare(&element, data_point_at(list, mid));

    if (cmp < 0) {
      right = mid;
    } else if (cmp > 0) {
      left = mid + 1;
    } else {
      return data_point_at(list, mid);
    }
  }

  return data_point_at(list, left);
}

