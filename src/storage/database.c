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

#include "src/utils/diagnostics.h"
#include "src/utils/memory.h"

sdb_data_series_t *sdb_database_get_data_series(sdb_database_t *db, sdb_data_series_id_t series_id);
sdb_data_series_t *sdb_database_create_data_series(sdb_database_t *db, sdb_data_series_id_t series_id);
sdb_data_series_t *sdb_database_get_or_create_data_series(sdb_database_t *db, sdb_data_series_id_t series_id);

sdb_database_t *sdb_database_create(const char *directory, int points_per_chunk) {
  sdb_assert(points_per_chunk > 1, "points_per_chunk must be greater than one")

  sdb_database_t *db = (sdb_database_t *)sdb_alloc(sizeof(sdb_database_t));
  strncpy(db->_directory, directory, SDB_FILE_MAX_LEN);
  db->_lock = sdb_rwlock_create();
  db->_max_series_count = 0;
  db->_points_per_chunk = points_per_chunk;
  db->_series = NULL;
  db->_series_count = 0;

  return db;
}

void sdb_database_destroy(sdb_database_t *db) {
  if (db->_series != NULL) {
    for (int i = 0; i < db->_series_count; i++) {
      sdb_log_info("closing time series: %d", db->_series[i]->id);
      sdb_data_series_destroy(db->_series[i]);
    }

    sdb_free(db->_series);
  }

  sdb_rwlock_destroy(db->_lock);
  sdb_free(db);
}

int sdb_database_write(sdb_database_t *db, sdb_data_series_id_t series_id, sdb_data_point_t *points, int count) {
  sdb_data_series_t *series = sdb_database_get_or_create_data_series(db, series_id);
  return sdb_data_series_write(series, points, count);
}

int sdb_database_truncate(sdb_database_t *db, sdb_data_series_id_t series_id) {
  sdb_data_series_t *series = sdb_database_get_or_create_data_series(db, series_id);
  return sdb_data_series_truncate(series);
}

sdb_data_points_reader_t *sdb_database_read(sdb_database_t *db, sdb_data_series_id_t series_id,
                                            sdb_timestamp_t begin,
                                            sdb_timestamp_t end,
                                            int max_points) {
  sdb_data_series_t *series = sdb_database_get_or_create_data_series(db, series_id);
  return sdb_data_series_read(series, begin, end, max_points);
}

sdb_data_series_t *sdb_database_get_data_series(sdb_database_t *db, sdb_data_series_id_t series_id) {
  if (db->_series == NULL) {
    return NULL;
  }

  sdb_data_series_t *series = NULL;

  for (int i = 0; i < db->_series_count && series == NULL; i++) {
    if (db->_series[i]->id == series_id) {
      series = db->_series[i];
    }
  }

  return series;
}

sdb_data_series_t *sdb_database_create_data_series(sdb_database_t *db, sdb_data_series_id_t series_id) {
  if (db->_series_count + 1 > db->_max_series_count) {
    db->_max_series_count += SDB_REALLOC_GROW_INCREMENT;
    db->_series = (sdb_data_series_t **)sdb_realloc(db->_series, sizeof(sdb_data_series_t *) * db->_max_series_count);
  }

  char file_name[SDB_FILE_MAX_LEN] = {0};
  snprintf(file_name, SDB_FILE_MAX_LEN, "%s/%d", db->_directory, series_id);

  sdb_log_info("loading time series: %d", series_id);
  return db->_series[db->_series_count++] = sdb_data_series_create(series_id, file_name, db->_points_per_chunk);
}

sdb_data_series_t *sdb_database_get_or_create_data_series(sdb_database_t *db, sdb_data_series_id_t series_id) {
  sdb_rwlock_rdlock(db->_lock);
  sdb_data_series_t *series = NULL;

  if ((series = sdb_database_get_data_series(db, series_id)) == NULL) {
    sdb_rwlock_upgrade(db->_lock);

    if ((series = sdb_database_get_data_series(db, series_id)) == NULL) {
      series = sdb_database_create_data_series(db, series_id);
    }
  }

  sdb_rwlock_unlock(db->_lock);
  return series;
}

