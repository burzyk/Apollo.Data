//
// Created by Pawel Burzynski on 16/04/2017.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include <getopt.h>

#include "src/client/client.h"
#include "src/common.h"
#include "src/utils/diagnostics.h"
#include "src/utils/memory.h"
#include "client.h"

#ifndef SDB_VERSION
#define SDB_VERSION "0.0.1"
#endif

#ifndef SDB_BUILD
#define SDB_BUILD "<COMMIT_ID>"
#endif

#define SDB_CLIENT_CMD_WRITE      "write"
#define SDB_CLIENT_CMD_READ       "read"
#define SDB_CLIENT_CMD_TRUNCATE   "truncate"
#define SDB_CLIENT_CMD_LATEST     "latest"

typedef struct sdb_client_configuration_s {
  char command[SDB_FILE_MAX_LEN];
  char hostname[SDB_FILE_MAX_LEN];
  int port;
  sdb_data_series_id_t series_id;
  sdb_timestamp_t begin;
  sdb_timestamp_t end;
} sdb_client_configuration_t;

void sdb_print_usage();
int sdb_client_configuration_load(int argc, char *argv[], sdb_client_configuration_t *config);
int sdb_execute_command(shakadb_session_t *session, sdb_client_configuration_t *config);

int main(int argc, char *argv[]) {
  sdb_client_configuration_t config;
  strcpy(config.command, "");
  strcpy(config.hostname, "localhost");
  config.port = 8487;
  config.series_id = 0;
  config.begin = 0;
  config.end = 0;

  if (sdb_client_configuration_load(argc, argv, &config)) {
    sdb_print_usage();
    return -1;
  }

  shakadb_session_t session = {};
  sdb_stopwatch_t *sw = sdb_stopwatch_start();

  if (shakadb_session_open(&session, config.hostname, config.port) != SHAKADB_RESULT_OK) {
    fprintf(stderr, "Failed to connect to: %s:%d\n", config.hostname, config.port);
    return -1;
  }

  fprintf(stderr, "Connected to: %s:%d\n", config.hostname, config.port);

  int status = sdb_execute_command(&session, &config);

  shakadb_session_close(&session);
  float elapsed = sdb_stopwatch_stop_and_destroy(sw);

  if (status == 0) {
    fprintf(stderr, "Command executed (%fs)\n", elapsed);
  } else {
    fprintf(stderr, "Command failed (%fs)\n", elapsed);
  }

  return status;
}

void sdb_print_usage() {
  printf("\n");
  printf("ShakaDB client - time series database client\n");
  printf("\n");
  printf("    Version:\t" SDB_VERSION "\n");
  printf("    Build:\t" SDB_BUILD "\n");
  printf("\n");
  printf("Usage: shakadb.client [options]\n");
  printf("\n");
  printf("    --command, -c:    Command to be executed. Command can be:\n");
  printf("                      write, read, truncate, latest\n");
  printf("    --hostname, -h:   ShakaDB network hostname\n");
  printf("                      Default value: 'localhost'\n");
  printf("    --port, -p:       ShakaDB network port\n");
  printf("                      Default value: 8487\n");
  printf("    --series, -s:     Time series id\n");
  printf("                      Default value: 0\n");
  printf("    --begin, -b:      Begin for reading, inclusive\n");
  printf("                      Default value: 0\n");
  printf("    --end, -e:        End for reading, exclusive\n");
  printf("                      Default value: 0\n");
  printf("\n");
  printf("For more info visit: http://shakadb.com/getting-started\n");
  printf("\n");
}

int sdb_client_configuration_load(int argc, char **argv, sdb_client_configuration_t *config) {
  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
        {"command", required_argument, 0, 'c'},
        {"hostname", required_argument, 0, 'h'},
        {"port", required_argument, 0, 'p'},
        {"series", required_argument, 0, 's'},
        {"begin", required_argument, 0, 'b'},
        {"end", required_argument, 0, 'e'}
    };

    int c = getopt_long(argc, argv, "c:m:p:s:b:e:", long_options, &option_index);
    if (c == -1) {
      return 0;
    }

    switch (c) {
      case 'c':strncpy(config->command, optarg, SDB_FILE_MAX_LEN);
        break;
      case 'h':strncpy(config->hostname, optarg, SDB_FILE_MAX_LEN);
        break;
      case 'p':config->port = atoi(optarg);
        break;
      case 's':config->series_id = (sdb_data_series_id_t)atoi(optarg);
        break;
      case 'b':sscanf(optarg, "%" PRIu64, &config->begin);
        break;
      case 'e':sscanf(optarg, "%" PRIu64, &config->end);
        break;
      default:return -1;
    }
  }
}
int sdb_execute_command(shakadb_session_t *session, sdb_client_configuration_t *config) {
  if (!strncmp(SDB_CLIENT_CMD_WRITE, config->command, SDB_FILE_MAX_LEN)) {
    fprintf(stderr, "writing to series: %d\n", config->series_id);
    int points_size = 65536;
    shakadb_data_point_t *points = (shakadb_data_point_t *)sdb_alloc(sizeof(shakadb_data_point_t *) * points_size);
    int read = 0;
    int data_available = 1;

    while (data_available) {
      data_available = !feof(stdin);

      if (data_available) {
        fscanf(stdin, "%" PRIu64 ",%f", &points[read].time, &points[read].value);
        read++;
      }

      if (read >= points_size || !data_available) {
        if (shakadb_write_points(session, config->series_id, points, read) != SHAKADB_RESULT_OK) {
          fprintf(stderr, "failed to write points\n");
          return -1;
        }

        read = 0;
      }
    }
  } else if (!strncmp(SDB_CLIENT_CMD_READ, config->command, SDB_FILE_MAX_LEN)) {
    fprintf(stderr,
            "reading points: %d -> [%" PRIu64 ", %" PRIu64 ")\n",
            config->series_id,
            config->begin,
            config->end);

    shakadb_data_points_iterator_t it;
    int total_read = 0;
    int s = shakadb_read_points(session, config->series_id, config->begin, config->end, SDB_POINTS_PER_PACKET_MAX, &it);

    if (s != SHAKADB_RESULT_OK) {
      fprintf(stderr, "failed to read data points\n");
      return -1;
    }

    while (shakadb_data_points_iterator_next(&it)) {
      total_read += it.points_count;

      for (int i = 0; i < it.points_count; i++) {
        printf("%" PRIu64 ",%f\n", it.points[i].time, it.points[i].value);
      }
    }

    fprintf(stderr, "total read: %d points\n", total_read);
  } else if (!strncmp(SDB_CLIENT_CMD_TRUNCATE, config->command, SDB_FILE_MAX_LEN)) {
    fprintf(stderr, "truncating data series: %d\n", config->series_id);

    if (shakadb_truncate_data_series(session, config->series_id) != SHAKADB_RESULT_OK) {
      fprintf(stderr, "failed to read data points\n");
      return -1;
    }

    fprintf(stderr, "truncate successful\n");
  } else if (!strncmp(SDB_CLIENT_CMD_LATEST, config->command, SDB_FILE_MAX_LEN)) {
    fprintf(stderr, "getting latest from: %d\n", config->series_id);

    shakadb_data_point_t latest = {0};

    if (shakadb_read_latest_point(session, config->series_id, &latest) != SHAKADB_RESULT_OK) {
      fprintf(stderr, "failed to get latest data point\n");
      return -1;
    }

    if (latest.time != 0) {
      printf("%" PRIu64 ",%f\n", latest.time, latest.value);
    } else {
      fprintf(stderr, "time series empty\n");
    }
  } else if (!strncmp("", config->command, SDB_FILE_MAX_LEN)) {
    sdb_print_usage();
  } else {
    fprintf(stderr, "unknown command: '%s'\n", config->command);
    return -1;
  }

  return 0;
}
