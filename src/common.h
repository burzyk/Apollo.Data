//
// Created by Pawel Burzynski on 14/04/2017.
//

#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t sdb_timestamp_t;
typedef uint32_t sdb_data_series_id_t;

#define SDB_TIMESTAMP_MIN ((sdb_timestamp_t)0)
#define SDB_TIMESTAMP_MAX ((sdb_timestamp_t)UINT64_MAX)

#define SDB_FILE_MAX_LEN  1024

#define SDB_REALLOC_GROW_INCREMENT 65536

typedef struct sdb_data_point_s {
  sdb_timestamp_t time;
  float value;
} __attribute__((packed)) sdb_data_point_t;

void die(const char *message);

#define sdb_min(a, b) ((a) < (b) ? (a) : (b))
#define sdb_max(a, b) ((a) < (b) ? (b) : (a))

#ifdef __cplusplus
}
#endif

#endif  // SRC_COMMON_H_
