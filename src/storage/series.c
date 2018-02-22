/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//
// Created by Pawel Burzynski on 28/01/2017.
//

#include "src/storage/series.h"

#include <string.h>

#include "src/diagnostics.h"
#include "src/storage/disk.h"

uint64_t series_prepare_input(data_point_t *points, uint64_t count);
void series_grow(series_t *series, uint64_t size);

series_t *series_create(const char *file_name) {
  file_map_t *map = file_map_create(file_name);

  if (map == NULL) {
    return NULL;
  }

  series_t *series = (series_t *)sdb_alloc(sizeof(series_t));
  strncpy(series->file_name, file_name, SDB_FILE_MAX_LEN);
  series->file_map = map;
  series->points = (data_point_t *)series->file_map->data;
  series->points_capacity = series->file_map->size / sizeof(data_point_t);
  series->points_count = series->points_capacity;

  if (series->file_map->size == 0) {
    series_grow(series, SDB_FILE_GROW_INCREMENT);
  }

  // TODO: use binary search here
  while (series->points_count > 1 && series->points[series->points_count - 1].time == 0) {
    series->points_count--;
  }

  return series;
}

void series_destroy(series_t *series) {
  file_map_destroy(series->file_map);
  sdb_free(series);
}

void series_truncate_and_destroy(series_t *series) {
  file_unlink(series->file_name);
  series_destroy(series);
}

int series_write(series_t *series, data_point_t *points, uint64_t count) {
  count = series_prepare_input(points, count);

  if (count == 0) {
    log_error("not available points to write");
    return -1;
  }

  if (series->points_count + count > series->points_capacity) {
    series_grow(series, count);
  }

  data_point_t latest = series_read_latest(series);

  if (points[0].time > latest.time) {
    memcpy(series->points + series->points_count, points, sizeof(data_point_t) * count);
    series->points_count += count;
  } else {
    data_point_t *begin = data_point_find(series->points, series->points_count, points[0].time);
    data_point_t *end = data_point_find(series->points, series->points_count, points[count - 1].time + 1);
    uint64_t slice_size = end - begin;

    data_point_t *merged = NULL;
    uint64_t merged_size = data_point_merge(points, (uint64_t)count, begin, slice_size, &merged);
    uint64_t tail_count = series->points + series->points_count - end;

    if (tail_count > 0) {
      memmove(begin + merged_size, end, tail_count * sizeof(data_point_t));
    }

    memcpy(begin, merged, merged_size * sizeof(data_point_t));
    series->points_count += merged_size - slice_size;
    sdb_free(merged);
  }

  file_map_sync(series->file_map);

  return 0;
}

uint64_t series_prepare_input(data_point_t *points, uint64_t count) {
  data_point_sort(points, count);
  uint64_t new_count = data_point_non_zero_distinct(points, count);

  if (new_count != count) {
    log_info("duplicated or zero timestamp points detected in request: %d -> %d", count, new_count);
  }

  return new_count;
}

points_reader_t *series_read(series_t *series, timestamp_t begin, timestamp_t end, uint64_t max_points) {
  data_point_t *begin_elem = data_point_find(series->points, series->points_count, begin);
  data_point_t *end_elem = data_point_find(series->points, series->points_count, end);
  uint64_t total_points = sdb_minl(max_points, end_elem - begin_elem);

  return points_reader_create(begin_elem, sdb_minl(max_points, total_points));
}

data_point_t series_read_latest(series_t *series) {
  data_point_t latest = {0};

  if (series->points_count != 0) {
    latest = series->points[series->points_count - 1];
  }

  return latest;
}

void series_grow(series_t *series, uint64_t size) {
  uint64_t increment = sdb_maxl(SDB_FILE_GROW_INCREMENT, size);

  file_map_destroy(series->file_map);
  file_grow(series->file_name, increment);

  series->points_capacity += increment;
  series->file_map = file_map_create(series->file_name);
  series->points = (data_point_t *)series->file_map->data;
}