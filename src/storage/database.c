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

sdb_data_series_t *sdb_database_get_data_series(sdb_database_t *db, sdb_data_series_id_t series_id);
sdb_data_series_t *sdb_database_create_data_series(sdb_database_t *db, sdb_data_series_id_t series_id);
sdb_data_series_t *sdb_database_get_or_create_data_series(sdb_database_t *db, sdb_data_series_id_t series_id);

sdb_database_t *sdb_database_create(const char *directory,
                                    int points_per_chunk,
                                    int max_series,
                                    uint64_t soft_limit,
                                    uint64_t hard_limit) {
  sdb_assert(points_per_chunk > 1, "points_per_chunk must be greater than one");

  sdb_database_t *db = (sdb_database_t *)sdb_alloc(sizeof(sdb_database_t));
  strncpy(db->_directory, directory, SDB_FILE_MAX_LEN);
  db->_max_series_count = max_series;
  db->_series = (sdb_data_series_t **)sdb_alloc(sizeof(sdb_data_series_t *) * db->_max_series_count);
  db->_points_per_chunk = points_per_chunk;
  db->_cache = cache_manager_create(soft_limit, hard_limit);

  return db;
}

void sdb_database_destroy(sdb_database_t *db) {
  for (int i = 0; i < db->_max_series_count; i++) {
    if (db->_series[i] == NULL) {
      continue;
    }

    log_info("closing time series: %d", db->_series[i]->id);
    sdb_data_series_destroy(db->_series[i]);
  }

  sdb_free(db->_series);

  cache_manager_destroy(db->_cache);
  sdb_free(db);
}

int sdb_database_write(sdb_database_t *db, sdb_data_series_id_t series_id, data_point_t *points, int count) {
  stopwatch_t *sw = stopwatch_start();

  sdb_data_series_t *series = sdb_database_get_or_create_data_series(db, series_id);
  int result = series == NULL ? -1 : sdb_data_series_write(series, points, count);
  log_debug("Written series: %d, points: %d in: %fs", series_id, count, stopwatch_stop_and_destroy(sw));

  return result;
}

int sdb_database_truncate(sdb_database_t *db, sdb_data_series_id_t series_id) {
  stopwatch_t *sw = stopwatch_start();

  sdb_data_series_t *series = sdb_database_get_or_create_data_series(db, series_id);
  int result = series == NULL ? -1 : sdb_data_series_truncate(series);
  log_debug("Truncated series: %d in: %fs", series_id, stopwatch_stop_and_destroy(sw));

  return result;
}

data_point_t sdb_database_read_latest(sdb_database_t *db, sdb_data_series_id_t series_id) {
  sdb_data_series_t *series = sdb_database_get_or_create_data_series(db, series_id);
  data_point_t result = {.time = 0, .value = 0};

  if (series != NULL) {
    result = sdb_data_series_read_latest(series);
  }

  return result;
}

points_reader_t *sdb_database_read(sdb_database_t *db, sdb_data_series_id_t series_id,
                                            timestamp_t begin,
                                            timestamp_t end,
                                            int max_points) {
  stopwatch_t *sw = stopwatch_start();

  sdb_data_series_t *series = sdb_database_get_or_create_data_series(db, series_id);
  points_reader_t *result = series == NULL
                                     ? points_reader_create(0)
                                     : sdb_data_series_read(series, begin, end, max_points);

  log_debug("Read series: %d, points: %d in: %fs",
            series_id,
            result->points_count,
            stopwatch_stop_and_destroy(sw));

  return result;
}

sdb_data_series_t *sdb_database_get_data_series(sdb_database_t *db, sdb_data_series_id_t series_id) {
  if (db->_series == NULL || series_id >= db->_max_series_count) {
    return NULL;
  }

  return db->_series[series_id];
}

sdb_data_series_t *sdb_database_create_data_series(sdb_database_t *db, sdb_data_series_id_t series_id) {
  if (series_id >= db->_max_series_count) {
    return NULL;
  }

  char file_name[SDB_FILE_MAX_LEN] = {0};
  snprintf(file_name, SDB_FILE_MAX_LEN, "%s/%d", db->_directory, series_id);

  log_info("loading time series: %d", series_id);
  return db->_series[series_id] = sdb_data_series_create(series_id, file_name, db->_points_per_chunk, db->_cache);
}

sdb_data_series_t *sdb_database_get_or_create_data_series(sdb_database_t *db, sdb_data_series_id_t series_id) {
  sdb_data_series_t *series = NULL;

  if ((series = sdb_database_get_data_series(db, series_id)) == NULL) {
      series = sdb_database_create_data_series(db, series_id);
  }

  return series;
}

