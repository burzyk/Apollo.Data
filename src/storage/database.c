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

#include "src/diagnostics.h"

series_t *database_get_or_create_data_series(database_t *db, series_id_t series_id);

database_t *database_create(const char *directory,
                            int points_per_chunk,
                            int max_series,
                            uint64_t soft_limit,
                            uint64_t hard_limit) {
  sdb_assert(points_per_chunk > 1, "points_per_chunk must be greater than one");

  database_t *db = (database_t *)sdb_alloc(sizeof(database_t));
  strncpy(db->directory, directory, SDB_FILE_MAX_LEN);
  db->max_series_count = max_series;
  db->series = (series_t **)sdb_alloc(sizeof(series_t *) * db->max_series_count);
  db->points_per_chunk = points_per_chunk;
  db->cache_manager = cache_manager_create(soft_limit, hard_limit);

  return db;
}

void database_destroy(database_t *db) {
  for (int i = 0; i < db->max_series_count; i++) {
    if (db->series[i] == NULL) {
      continue;
    }

    log_info("closing time series: %d", db->series[i]->id);
    series_destroy(db->series[i]);
  }

  sdb_free(db->series);

  cache_manager_destroy(db->cache_manager);
  sdb_free(db);
}

int database_write(database_t *db, series_id_t series_id, data_point_t *points, int count) {
  stopwatch_t *sw = stopwatch_start();

  series_t *series = database_get_or_create_data_series(db, series_id);
  int result = series == NULL ? -1 : series_write(series, points, count);
  log_debug("Written series: %d, points: %d in: %fs", series_id, count, stopwatch_stop_and_destroy(sw));

  return result;
}

int database_truncate(database_t *db, series_id_t series_id) {
  stopwatch_t *sw = stopwatch_start();

  series_t *series = database_get_or_create_data_series(db, series_id);
  int result = series == NULL ? -1 : series_truncate(series);
  log_debug("Truncated series: %d in: %fs", series_id, stopwatch_stop_and_destroy(sw));

  return result;
}

data_point_t database_read_latest(database_t *db, series_id_t series_id) {
  series_t *series = database_get_or_create_data_series(db, series_id);
  data_point_t result = {.time = 0, .value = 0};

  if (series != NULL) {
    result = series_read_latest(series);
  }

  return result;
}

points_reader_t *database_read(database_t *db,
                               series_id_t series_id,
                               timestamp_t begin,
                               timestamp_t end,
                               int max_points) {
  stopwatch_t *sw = stopwatch_start();

  series_t *series = database_get_or_create_data_series(db, series_id);
  points_reader_t *result = series == NULL ? points_reader_create(0) : series_read(series, begin, end, max_points);

  log_debug("Read series: %d, points: %d in: %fs",
            series_id,
            result->points_count,
            stopwatch_stop_and_destroy(sw));

  return result;
}

series_t *database_get_or_create_data_series(database_t *db, series_id_t series_id) {
  if (db->series == NULL || series_id >= db->max_series_count) {
    return NULL;
  }

  series_t *series = NULL;

  if ((series = db->series[series_id]) == NULL) {
    char file_name[SDB_FILE_MAX_LEN] = {0};
    snprintf(file_name, SDB_FILE_MAX_LEN, "%s/%d", db->directory, series_id);

    log_info("loading time series: %d", series_id);
    series = db->series[series_id] = series_create(series_id, file_name, db->points_per_chunk, db->cache_manager);
  }

  return series;
}

