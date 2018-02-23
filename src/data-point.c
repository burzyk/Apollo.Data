//
// Created by Pawel Burzynski on 22/02/2018.
//

#include <stdlib.h>
#include <string.h>

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

void data_point_sort(points_list_t *list) {
  qsort(list->points, list->count, list->point_size, (int (*)(const void *, const void *))data_point_compare);
}

uint64_t data_point_non_zero_distinct(points_list_t *list) {
  data_point_t *curr = list->points;
  data_point_t *tail = data_point_prev(list, curr);
  data_point_t *end = points_list_end(list);

  while (curr < end) {
    if (curr->time != 0) {
      if (tail < list->points || tail->time < curr->time) {
        tail = data_point_next(list, tail);
      }

      memcpy(tail, curr, list->point_size);
    }

    curr = data_point_next(list, curr);
  }

  tail = data_point_next(list, tail);
  return data_point_dist(list, list->points, tail);
}

inline data_point_t *data_point_at(points_list_t *list, uint64_t offset) {
  return (data_point_t *)(((uint8_t *)list->points) + offset * list->point_size);
}

inline data_point_t *data_point_next(points_list_t *list, data_point_t *curr) {
  return (data_point_t *)(((uint8_t *)curr) + list->point_size);
}

inline data_point_t *data_point_prev(points_list_t *list, data_point_t *curr) {
  return (data_point_t *)(((uint8_t *)curr) - list->point_size);
}

inline data_point_t *points_list_end(points_list_t *list) {
  return data_point_at(list, list->count);
}

inline uint64_t data_point_dist(points_list_t *list, data_point_t *start, data_point_t *end) {
  return (((uint8_t *)end) - ((uint8_t *)start)) / list->point_size;
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
    return points_list_end(list);
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

