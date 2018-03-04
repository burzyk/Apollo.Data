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
// Created by Pawel Burzynski on 17/01/2017.
//

#include "src/storage/database.h"

#include <memory.h>
#include <dirent.h>
#include <stdlib.h>

#include "src/diagnostics.h"

series_t *database_get_or_create_data_series(database_t *db, series_id_t series_id, uint32_t point_size);
series_t *database_get_or_load_data_series(database_t *db, series_id_t series_id);

database_t *database_create(const char *directory, uint64_t max_series, int no_flush) {
  database_t *db = (database_t *)sdb_alloc(sizeof(database_t));
  strncpy(db->directory, directory, SDB_STR_MAX_LEN);
  db->max_series_count = max_series;
  db->series = (series_t **)sdb_alloc(sizeof(series_t *) * db->max_series_count);
  db->no_flush = no_flush;

  return db;
}

void database_destroy(database_t *db) {
  for (int i = 0; i < db->max_series_count; i++) {
    if (db->series[i] == NULL) {
      continue;
    }

    log_info("closing time series: %d", i);
    series_destroy(db->series[i]);
  }

  sdb_free(db->series);
  sdb_free(db);
}

int database_write(database_t *db, series_id_t series_id, points_list_t *points) {
  stopwatch_t *sw = stopwatch_start();

  series_t *series = database_get_or_create_data_series(db, series_id, points->point_size);
  int result = series == NULL ? -1 : series_write(series, points);

  log_debug("Written series: %d, points: %d in: %fs", series_id, points->count, stopwatch_stop_and_destroy(sw));
  return result;
}

int database_truncate(database_t *db, series_id_t series_id) {
  stopwatch_t *sw = stopwatch_start();

  series_t *series = database_get_or_load_data_series(db, series_id);

  if (series == NULL) {
    return 0;
  }

  series_truncate_and_destroy(series);
  db->series[series_id] = NULL;

  log_debug("Truncated series: %d in: %fs", series_id, stopwatch_stop_and_destroy(sw));

  return 0;
}

uint32_t database_get_point_size(database_t *db, series_id_t series_id) {
  series_t *series = database_get_or_load_data_series(db, series_id);
  return series == NULL ? 0 : series->points.point_size;
}

points_reader_t *database_read_latest(database_t *db, series_id_t series_id) {
  stopwatch_t *sw = stopwatch_start();

  series_t *series = database_get_or_load_data_series(db, series_id);
  points_reader_t *result = series == NULL
                            ? points_reader_create(NULL, 0, 0)
                            : series_read_latest(series);

  log_debug("Read latest from series: %d in: %fs", series_id, stopwatch_stop_and_destroy(sw));

  return result;
}

points_reader_t *database_read(database_t *db,
                               series_id_t series_id,
                               timestamp_t begin,
                               timestamp_t end,
                               uint64_t max_points) {
  stopwatch_t *sw = stopwatch_start();

  series_t *series = database_get_or_load_data_series(db, series_id);
  points_reader_t *result = series == NULL
                            ? points_reader_create(NULL, 0, 0)
                            : series_read(series, begin, end, max_points);

  log_debug("Read series: %d, points: %d in: %fs",
            series_id,
            result->points.count,
            stopwatch_stop_and_destroy(sw));

  return result;
}

series_t *database_get_or_create_data_series(database_t *db, series_id_t series_id, uint32_t point_size) {
  if (db->series == NULL || series_id >= db->max_series_count) {
    return NULL;
  }

  series_t *series = database_get_or_load_data_series(db, series_id);

  if (series != NULL) {
    return series;
  }

  char file_name[SDB_STR_MAX_LEN] = {0};
  snprintf(file_name, SDB_STR_MAX_LEN, "%s/%d-%d", db->directory, series_id, point_size);

  log_info("loading or creating time series: %d", series_id);
  return db->series[series_id] = series_create(file_name, point_size, db->no_flush);
}

series_t *database_get_or_load_data_series(database_t *db, series_id_t series_id) {
  if (db->series == NULL || series_id >= db->max_series_count) {
    return NULL;
  }

  series_t *series = db->series[series_id];

  if (series != NULL) {
    return series;
  }

  char file_root[SDB_STR_MAX_LEN] = {0};
  snprintf(file_root, SDB_STR_MAX_LEN, "%d-", series_id);

  DIR *d = opendir(db->directory);

  if (d == NULL) {
    return NULL;
  }

  struct dirent *dir;
  size_t root_len = strlen(file_root);

  while ((dir = readdir(d)) != NULL && series == NULL) {
    size_t dir_name_len = strlen(dir->d_name);

    if (root_len >= dir_name_len || strncmp(file_root, dir->d_name, root_len) != 0) {
      continue;
    }

    log_info("loading time series: %d", series_id);
    uint32_t point_size = (uint32_t)strtol(&dir->d_name[root_len], NULL, 0);
    char file_name[SDB_STR_MAX_LEN] = {0};
    snprintf(file_name, SDB_STR_MAX_LEN, "%s/%s", db->directory, dir->d_name);

    series = db->series[series_id] = series_create(file_name, point_size, db->no_flush);
  }

  closedir(d);

  return series;
}

