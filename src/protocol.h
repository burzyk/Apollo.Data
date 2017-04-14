//
// Created by Pawel Burzynski on 14/04/2017.
//

#ifndef SRC_PROTOCOL_H_
#define SRC_PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>

#include "src/common.h"

#include "src/utils/network.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef char sdb_packet_type_t;
typedef char sdb_response_code_t;

#define SDB_RESPONSE_OK ((sdb_response_code_t)0)
#define SDB_RESPONSE_ERROR  ((sdb_response_code_t)1)

#define SDB_WRITE_REQUEST ((sdb_packet_type_t)1)
#define SDB_WRITE_RESPONSE  ((sdb_packet_type_t)2)
#define SDB_READ_REQUEST ((sdb_packet_type_t)3)
#define SDB_READ_RESPONSE  ((sdb_packet_type_t)4)

#define SDB_PACKET_MAX_LEN  6553600

typedef struct sdb_packet_header_s {
  sdb_packet_type_t type;
  size_t payload_size;
} sdb_packet_header_t;

typedef struct sdb_packet_s {
  sdb_packet_header_t header;
  void *payload;
  void *raw_payload;
} sdb_packet_t;

typedef struct sdb_write_request_s {
  sdb_data_series_id_t data_series_id;
  int points_count;
  sdb_data_point_t *points;
} sdb_write_request_t;

typedef struct sdb_write_response_s {
  sdb_response_code_t code;
} sdb_write_response_t;

typedef struct sdb_read_request_s {
  sdb_data_series_id_t data_series_id;
  sdb_timestamp_t begin;
  sdb_timestamp_t end;
} sdb_read_request_t;

typedef struct sdb_read_response_s {
  sdb_response_code_t code;
  int points_count;
  sdb_data_point_t *points;
} sdb_read_response_t;

sdb_packet_t *sdb_write_request_create(sdb_data_series_id_t data_series_id, sdb_data_point_t *points, int points_count);
sdb_packet_t *sdb_write_response_create(sdb_response_code_t code);
sdb_packet_t *sdb_read_request_create(sdb_data_series_id_t data_series_id, sdb_timestamp_t begin, sdb_timestamp_t end);
sdb_packet_t *sdb_read_response_create(sdb_response_code_t code, sdb_data_point_t *points, int points_count);

sdb_packet_t *sdb_packet_receive(sdb_socket_t socket);
int sdb_packet_send(sdb_packet_t *packet, sdb_socket_t socket);
int sdb_packet_send_and_destroy(sdb_packet_t *packet, sdb_socket_t socket);
void sdb_packet_destroy(sdb_packet_t *packet);

#ifdef __cplusplus
}
#endif

#endif  // SRC_PROTOCOL_H_
