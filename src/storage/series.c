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

uint64_t series_prepare_input(points_list_t *list);
void series_grow(series_t *series, uint64_t size);

series_t *series_create(const char *file_name, uint32_t point_size) {
  file_map_t *map = file_map_create(file_name);

  if (map == NULL) {
    return NULL;
  }

  series_t *series = (series_t *)sdb_alloc(sizeof(series_t));
  strncpy(series->file_name, file_name, SDB_FILE_MAX_LEN);
  series->file_map = map;
  series->points.content = (data_point_t *)series->file_map->data;
  series->points.point_size = point_size;
  series->points_capacity = series->file_map->size / series->points.point_size;

  if (series->file_map->size == 0) {
    series_grow(series, SDB_FILE_GROW_INCREMENT);
  }

  // just for now to do the binary search
  series->points.count = series->points_capacity;
  data_point_t *end = data_point_find(&series->points, 0xFFFFFFFFFFFFFFFE);

  series->points.count = data_point_dist(&series->points, series->points.content, end);

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

int series_write(series_t *series, points_list_t *to_write) {
  if (to_write->point_size != series->points.point_size) {
    log_error("incompatible point size");
    return -1;
  }

  to_write->count = series_prepare_input(to_write);

  if (to_write->count == 0) {
    log_error("not available points to write");
    return -1;
  }

  if (series->points.count + to_write->count > series->points_capacity) {
    series_grow(series, to_write->count);
  }

  timestamp_t latest = series->points.count == 0
                       ? 0
                       : data_point_at(&series->points, series->points.count - 1)->time;

  if (to_write->content->time > latest) {
    memcpy(points_list_end(&series->points), to_write->content, to_write->point_size * to_write->count);
    series->points.count += to_write->count;
  } else {
    data_point_t *begin = data_point_find(&series->points, to_write->content->time);
    data_point_t *end = data_point_find(&series->points, points_list_last(to_write)->time + 1);
    points_list_t slice = {
        .content = begin,
        .count = data_point_dist(&series->points, begin, end),
        .point_size = to_write->point_size
    };

    points_list_t merged = {};
    merged.count = slice.count + to_write->count;
    merged.point_size = series->points.point_size;
    merged.content = (data_point_t *)sdb_alloc(merged.count * merged.point_size);

    data_point_merge(to_write, &slice, &merged);
    uint64_t tail_count = data_point_dist(&series->points, end, points_list_end(&series->points));

    if (tail_count > 0) {
      memmove(
          data_point_at(&series->points, data_point_index(&series->points, begin) + merged.count),
          end,
          tail_count * series->points.point_size);
    }

    memcpy(begin, merged.content, merged.count * merged.point_size);
    series->points.count += merged.count - slice.count;
    sdb_free(merged.content);
  }

  file_map_sync(series->file_map);

  return 0;
}

uint64_t series_prepare_input(points_list_t *list) {
  data_point_sort(list);
  uint64_t new_count = data_point_non_zero_distinct(list);

  if (new_count != list->count) {
    log_info("duplicated or zero timestamp points detected in request: %d -> %d", list->count, new_count);
  }

  return new_count;
}

points_reader_t *series_read(series_t *series, timestamp_t begin, timestamp_t end, uint64_t max_points) {
  data_point_t *begin_elem = data_point_find(&series->points, begin);
  data_point_t *end_elem = data_point_find(&series->points, end);
  uint64_t total_points = sdb_min(max_points, end_elem - begin_elem);

  return points_reader_create(begin_elem, sdb_min(max_points, total_points));
}

points_reader_t *series_read_latest(series_t *series) {
  data_point_t *latest = series->points.count == 0 ? NULL : data_point_at(&series->points, series->points.count - 1);
  points_reader_t *reader = points_reader_create(latest, latest == NULL ? 0 : 1);

  return reader;
}

void series_grow(series_t *series, uint64_t size) {
  uint64_t increment = sdb_max(SDB_FILE_GROW_INCREMENT, size);

  file_map_destroy(series->file_map);
  file_grow(series->file_name, increment * series->points.point_size, 0xFF);

  series->points_capacity += increment;
  series->file_map = file_map_create(series->file_name);
  series->points.content = (data_point_t *)series->file_map->data;
}