//
// Created by Pawel Burzynski on 16/04/2017.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>

#include "src/client/client.h"
#include "src/common.h"

#ifndef SDB_VERSION
#define SDB_VERSION "0.0.1"
#endif

#ifndef SDB_BUILD
#define SDB_BUILD "<COMMIT_ID>"
#endif

void sdb_print_usage();

int main(int argc, char *argv[]) {
  if (argc < 4) {
    sdb_print_usage();
    return -1;
  }

  const char *hostname = argv[1];
  int port = atoi(argv[2]);

  shakadb_session_t session = {};

  if (shakadb_session_open(&session, hostname, port) != SHAKADB_RESULT_OK) {
    fprintf(stderr, "Failed to connect to: %s:%d\n", hostname, port);
    return -1;
  }

  printf("Connected to: %s:%d\n", hostname, port);
  const char *command = argv[3];

  if (!strcmp("write", command)) {
    shakadb_data_series_id_t series_id = (shakadb_data_series_id_t)atoi(argv[4]);
    shakadb_data_point_t data_point = {
        .time =(shakadb_timestamp_t)atoi(argv[5]),
        .value=(float)atof(argv[6])
    };

    printf("writing point: %d -> (%" PRIu64 ", %f)\n", series_id, data_point.time, data_point.value);

    if (shakadb_write_points(&session, series_id, &data_point, 1) != SHAKADB_RESULT_OK) {
      fprintf(stderr, "Failed to write the point\n");
      return -1;
    }
  }

  if (!strcmp("read", command)) {
    shakadb_data_series_id_t series_id = (shakadb_data_series_id_t)atoi(argv[4]);
    shakadb_timestamp_t begin;
    shakadb_timestamp_t end;

    sscanf(argv[5], "%" PRIu64, &begin);
    sscanf(argv[6], "%" PRIu64, &end);

    printf("reading points: %d -> [%" PRIu64 ", %" PRIu64 ")\n", series_id, begin, end);

    shakadb_data_points_iterator_t it;
    int total_read = 0;

    if (shakadb_read_points(&session, series_id, begin, end, SDB_POINTS_PER_PACKET_MAX, &it) != SHAKADB_RESULT_OK) {
      fprintf(stderr, "Failed to read data points\n");
      return -1;
    }

    while (shakadb_data_points_iterator_next(&it)) {
      total_read += it.points_count;

      for (int i = 0; i < it.points_count; i++) {
        printf("(%" PRIu64 ", %f)\n", it.points[i].time, it.points[i].value);
      }
    }

    printf("Total read: %d points\n", total_read);
  }

  if (!strcmp("truncate", command)) {
    shakadb_data_series_id_t series_id = (shakadb_data_series_id_t)atoi(argv[4]);

    printf("truncating data series: %d\n", series_id);

    if (shakadb_truncate_data_series(&session, series_id) != SHAKADB_RESULT_OK) {
      fprintf(stderr, "Failed to read data points\n");
      return -1;
    }

    printf("truncate successful\n");
  }

  shakadb_session_close(&session);
  printf("Disconnected\n");

  return 0;
}

void sdb_print_usage() {
  printf("\n");
  printf("ShakaDB client - time series database client\n");
  printf("\n");
  printf("    Version:\t" SDB_VERSION "\n");
  printf("    Build:\t" SDB_BUILD "\n");
  printf("\n");
  printf("Usage: shakadb.client <server> <port> <command> <options>\n");
  printf("\n");
  printf("    server:           ShakaDB server hostname to connect\n");
  printf("    port:             ShakaDB server port to connect\n");
  printf("    command:          Command to be executed\n");
  printf("        write <series_id> <timestamp> <value>\n");
  printf("        read  <series_id> <begin> <end>\n");
  printf("        truncate <series_id>\n");
  printf("\n");
  printf("For more info visit: http://shakadb.com/getting-started\n");
  printf("\n");
}
