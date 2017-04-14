//
// Created by Pawel Burzynski on 14/04/2017.
//

#ifndef SRC_C_COMMON_H_
#define SRC_C_COMMON_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t sdb_timestamp_t;
typedef uint32_t sdb_data_series_id_t;

#define SDB_TIMESTAMP_MIN 0;
#define SDB_TIMESTAMP_MAX UINT64_MAX;

typedef struct sdb_data_point_s {
  sdb_timestamp_t time;
  float value;
} __attribute__((packed)) sdb_data_point_t;

void die(const char *message);

sdb_timestamp_t sdb_min(sdb_timestamp_t a, sdb_timestamp_t b);
sdb_timestamp_t sdb_max(sdb_timestamp_t a, sdb_timestamp_t b);

#ifdef __cplusplus
}
#endif

#endif  // SRC_C_COMMON_H_
