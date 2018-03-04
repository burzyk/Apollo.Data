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

void data_point_merge(points_list_t *src, points_list_t *dst, points_list_t *result) {
  uint64_t buffer_count = result->count;
  uint32_t point_size = result->point_size;
  data_point_t *buffer = result->content;
  data_point_t *src_pos = src->content;
  data_point_t *dst_pos = dst->content;
  data_point_t *src_end = points_list_end(src);
  data_point_t *dst_end = points_list_end(dst);
  uint64_t processed_count = 0;
  uint64_t duplicated_count = 0;

  while (processed_count < buffer_count - duplicated_count) {
    if (src_pos == src_end) {
      memmove(buffer, dst_pos, point_size);
      dst_pos = data_point_next(dst, dst_pos);

    } else if (dst_pos == dst_end) {
      memmove(buffer, src_pos, point_size);
      src_pos = data_point_next(src, src_pos);

    } else if (dst_pos->time < src_pos->time) {
      memmove(buffer, dst_pos, point_size);
      dst_pos = data_point_next(dst, dst_pos);

    } else if (src_pos->time < dst_pos->time) {
      memmove(buffer, src_pos, point_size);
      src_pos = data_point_next(src, src_pos);

    } else {
      memmove(buffer, src_pos, point_size);
      src_pos = data_point_next(src, src_pos);
      dst_pos = data_point_next(dst, dst_pos);
      duplicated_count++;
    }

    buffer = data_point_next(result, buffer);
    processed_count++;
  }

  result->count = buffer_count - duplicated_count;
}

void data_point_sort(points_list_t *points) {
  qsort(points->content, points->count, points->point_size, (int (*)(const void *, const void *))data_point_compare);
}

uint64_t data_point_non_zero_distinct(points_list_t *points) {
  data_point_t *curr = points->content;
  data_point_t *tail = data_point_prev(points, curr);
  data_point_t *end = points_list_end(points);

  while (curr < end) {
    if (curr->time != 0) {
      if (tail < points->content || tail->time < curr->time) {
        tail = data_point_next(points, tail);
      }

      memcpy(tail, curr, points->point_size);
    }

    curr = data_point_next(points, curr);
  }

  tail = data_point_next(points, tail);
  return data_point_index(points, tail);
}

inline data_point_t *data_point_at(points_list_t *points, uint64_t offset) {
  return (data_point_t *)(((uint8_t *)points->content) + offset * points->point_size);
}

uint64_t data_point_index(points_list_t *points, data_point_t *point) {
  return data_point_dist(points, points->content, point);
}

inline data_point_t *data_point_next(points_list_t *points, data_point_t *curr) {
  return (data_point_t *)(((uint8_t *)curr) + points->point_size);
}

inline data_point_t *data_point_prev(points_list_t *points, data_point_t *curr) {
  return (data_point_t *)(((uint8_t *)curr) - points->point_size);
}

inline data_point_t *points_list_end(points_list_t *points) {
  return data_point_at(points, points->count);
}

inline uint64_t data_point_dist(points_list_t *points, data_point_t *start, data_point_t *end) {
  return (((uint8_t *)end) - ((uint8_t *)start)) / points->point_size;
}

inline data_point_t *points_list_last(points_list_t *points) {
  return data_point_at(points, points->count - 1);
}

data_point_t *data_point_find(points_list_t *points, timestamp_t timestamp) {
  if (points->content == NULL || points->count == 0) {
    return NULL;
  }

  uint64_t left = 0;
  uint64_t right = points->count;
  data_point_t element = {.time = timestamp};

  if (data_point_compare(&element, points->content) < 0) {
    return points->content;
  }

  if (data_point_compare(&element, points_list_last(points)) > 0) {
    return points_list_end(points);
  }

  while (left < right) {
    uint64_t mid = (right + left) / 2;
    int cmp = data_point_compare(&element, data_point_at(points, mid));

    if (cmp < 0) {
      right = mid;
    } else if (cmp > 0) {
      left = mid + 1;
    } else {
      return data_point_at(points, mid);
    }
  }

  return data_point_at(points, left);
}

