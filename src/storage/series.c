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
#include <inttypes.h>
#include <stdlib.h>

#include "src/diagnostics.h"
#include "src/storage/disk.h"

uint64_t series_prepare_input(data_point_t *points, uint64_t count);

series_t *series_create(const char *file_name) {
  series_t *series = (series_t *)sdb_alloc(sizeof(series_t));
  strncpy(series->file_name, file_name, SDB_FILE_MAX_LEN);
  series->points = (data_point_t *)file_map(series->file_name);
  series->points_capacity = file_size(series->file_name) / sizeof(data_point_t);
  series->points_count = series->points_capacity;

  while (series->points_count > 1 && series->points[series->points_count - 1].time == 0) {
    series->points_count--;
  }

  return series;
}

void series_destroy(series_t *series) {
  file_unmap(series->points);
  sdb_free(series);
}

int series_write(series_t *series, data_point_t *points, int count) {
  count = series_prepare_input(points, count);

  if (count == 0) {
    log_error("not available points to write");
    return -1;
  }

  if (series->points_count + count > series->points_capacity) {
    uint64_t increment = sdb_maxl(SDB_FILE_GROW_INCREMENT, count);
    file_unmap(series->points);
    file_grow(series->file_name, increment);
    series->points_capacity += increment;
    series->points = (data_point_t *)file_map(series->file_name);
  }

  data_point_t latest = series_read_latest(series);

  if (points[0].time > latest.time) {
    memcpy(series->points + series->points_count, points, sizeof(data_point_t) * count);
    series->points_count += count;
  } else {
    find_predicate cmp = (find_predicate)data_point_compare;
    int elem_size = sizeof(data_point_t);

    uint64_t begin = sdb_find(series->points, elem_size, series->points_count, &points[0], cmp);
    uint64_t end = sdb_find(series->points, elem_size, series->points_count, &points[count - 1], cmp);
    uint64_t slice_size = end - begin;

    data_point_t *merged = NULL;
    uint64_t merged_size = data_point_merge(points, (uint64_t)count, &series->points[begin], slice_size, &merged);

    if (end < series->points_count) {
      memmove(series->points + begin + merged_size,
              series->points + end,
              (series->points_count - end) * sizeof(data_point_t));
    }

    memcpy(series->points + begin, merged, merged_size * sizeof(data_point_t));
    series->points_count += merged_size - slice_size;
    sdb_free(merged);
  }

  file_sync(series->points);

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

int series_truncate(series_t *series) {
  file_unmap(series->points);
  file_unlink(series->file_name);

  return 0;
}

points_reader_t *series_read(series_t *series, timestamp_t begin, timestamp_t end, int max_points) {
  find_predicate cmp = (find_predicate)data_point_compare;
  int elem_size = sizeof(data_point_t);

  uint64_t begin_index = sdb_find(series->points, elem_size, series->points_count, &begin, cmp);
  uint64_t end_index = sdb_find(series->points, elem_size, series->points_count, &end, cmp);
  uint64_t total_points = sdb_minl(max_points, end_index - begin_index);

  return points_reader_create(series->points + begin_index, sdb_min(max_points, total_points));
}

data_point_t series_read_latest(series_t *series) {
  data_point_t latest = {0};

  if (series->points_count != 0) {
    latest = series->points[series->points_count - 1];
  }

  return latest;
}
