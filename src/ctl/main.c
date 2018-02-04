//
// Created by Pawel Burzynski on 16/04/2017.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <inttypes.h>
#include <getopt.h>

#include "src/common.h"
#include "src/diagnostics.h"
#include "src/ctl/session.h"

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
#define SDB_CLIENT_CMD_TO_CSV     "to_csv"
#define SDB_CLIENT_CMD_FROM_CSV   "from_csv"

typedef struct client_configuration_s {
  char command[SDB_FILE_MAX_LEN];
  char hostname[SDB_FILE_MAX_LEN];
  int port;
  series_id_t series_id;
  timestamp_t begin;
  timestamp_t end;
} client_configuration_t;

void print_usage();
int client_configuration_load(int argc, char **argv, client_configuration_t *config);
int execute_command(session_t *session, client_configuration_t *config);
int execute_write(session_t *session, client_configuration_t *config);
int execute_read(session_t *session, client_configuration_t *config);
int execute_truncate(session_t *session, client_configuration_t *config);
int execute_get_latest(session_t *session, client_configuration_t *config);
int execute_from_csv(session_t *session, client_configuration_t *config);
int execute_to_csv(session_t *session, client_configuration_t *config);

int main(int argc, char *argv[]) {
  client_configuration_t config;
  strcpy(config.command, "");
  strcpy(config.hostname, "localhost");
  config.port = 8487;
  config.series_id = 0;
  config.begin = 0;
  config.end = 0;

  if (client_configuration_load(argc, argv, &config)) {
    print_usage();
    return -1;
  }

  session_t *session = NULL;
  stopwatch_t *sw = stopwatch_start();

  if ((session = session_create(config.hostname, config.port)) == NULL) {
    fprintf(stderr, "Failed to connect to: %s:%d\n", config.hostname, config.port);
    return -1;
  }

  fprintf(stderr, "Connected to: %s:%d\n", config.hostname, config.port);

  int status = execute_command(session, &config);

  session_destroy(session);
  float elapsed = stopwatch_stop_and_destroy(sw);

  if (status == 0) {
    fprintf(stderr, "Command executed (%fs)\n", elapsed);
  } else {
    fprintf(stderr, "Command failed (%fs)\n", elapsed);
  }

  return status;
}

void print_usage() {
  printf("\n");
  printf("shakadbctl - controls shakadb instance\n");
  printf("\n");
  printf("    Version:\t" SDB_VERSION "\n");
  printf("    Build:\t" SDB_BUILD "\n");
  printf("\n");
  printf("Usage: shakadbctl [options]\n");
  printf("\n");
  printf("    --command, -c:    Command to be executed. Command can be:\n");
  printf("                      write, read, truncate, latest, to_csv, from_csv\n");
  printf("    --hostname, -h:   ShakaDB server hostname\n");
  printf("                      Default value: 'localhost'\n");
  printf("    --port, -p:       ShakaDB server port\n");
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

int client_configuration_load(int argc, char **argv, client_configuration_t *config) {
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
      case 's':config->series_id = (series_id_t)atoi(optarg);
        break;
      case 'b':sscanf(optarg, "%" PRIu64, &config->begin);
        break;
      case 'e':sscanf(optarg, "%" PRIu64, &config->end);
        break;
      default:return -1;
    }
  }
}

int execute_command(session_t *session, client_configuration_t *config) {
  if (!strncmp(SDB_CLIENT_CMD_WRITE, config->command, SDB_FILE_MAX_LEN)) {
    return execute_write(session, config);

  } else if (!strncmp(SDB_CLIENT_CMD_READ, config->command, SDB_FILE_MAX_LEN)) {
    return execute_read(session, config);

  } else if (!strncmp(SDB_CLIENT_CMD_TRUNCATE, config->command, SDB_FILE_MAX_LEN)) {
    return execute_truncate(session, config);

  } else if (!strncmp(SDB_CLIENT_CMD_LATEST, config->command, SDB_FILE_MAX_LEN)) {
    return execute_get_latest(session, config);

  } else if (!strncmp(SDB_CLIENT_CMD_TO_CSV, config->command, SDB_FILE_MAX_LEN)) {
    return execute_to_csv(session, config);

  } else if (!strncmp(SDB_CLIENT_CMD_FROM_CSV, config->command, SDB_FILE_MAX_LEN)) {
    return execute_from_csv(session, config);

  } else if (!strncmp("", config->command, SDB_FILE_MAX_LEN)) {
    print_usage();
  } else {
    fprintf(stderr, "unknown command: '%s'\n", config->command);
    return -1;
  }

  return 0;
}

int execute_write(session_t *session, client_configuration_t *config) {

  fprintf(stderr, "writing to series: %d\n", config->series_id);
  int points_size = 6553600;
  data_point_t *points = (data_point_t *)sdb_alloc(sizeof(data_point_t) * points_size);

  while (!feof(stdin)) {
    size_t read = fread(points, sizeof(data_point_t), (size_t)points_size, stdin);

    if (read > 0) {
      if (session_write(session, config->series_id, points, (int)read)) {
        fprintf(stderr, "failed to write points\n");
        return -1;
      }
    }
  }

  sdb_free(points);
  return 0;
}

int execute_read(session_t *session, client_configuration_t *config) {
  fprintf(stderr,
          "reading points: %d -> [%" PRIu64 ", %" PRIu64 ")\n",
          config->series_id,
          config->begin,
          config->end);

  int total_read = 0;

  if (session_read(session, config->series_id, config->begin, config->end, SDB_POINTS_PER_PACKET_MAX)) {
    fprintf(stderr, "failed to read data points\n");
    return -1;
  }

  while (session_read_next(session)) {
    total_read += session->read_response->points_count;

    fwrite(
        session->read_response->points,
        sizeof(data_point_t),
        (size_t)session->read_response->points_count,
        stdout);
  }

  fprintf(stderr, "total read: %d points\n", total_read);

  return 0;
}

int execute_truncate(session_t *session, client_configuration_t *config) {

  fprintf(stderr, "truncating data series: %d\n", config->series_id);

  if (session_truncate(session, config->series_id)) {
    fprintf(stderr, "failed to read data points\n");
    return -1;
  }

  fprintf(stderr, "truncate successful\n");

  return 0;
}

int execute_get_latest(session_t *session, client_configuration_t *config) {

  fprintf(stderr, "getting latest from: %d\n", config->series_id);
  data_point_t latest = {0};

  if (session_read_latest(session, config->series_id, &latest)) {
    fprintf(stderr, "failed to get latest data point\n");
    return -1;
  }

  if (latest.time != 0) {
    printf("%" PRIu64 ",%f\n", latest.time, latest.value);
  } else {
    fprintf(stderr, "time series empty\n");
  }

  return 0;
}

int execute_from_csv(session_t *session, client_configuration_t *config) {
  fprintf(stderr, "converting to binary\n");
  timestamp_t time;
  float value;

  while (!feof(stdin)) {
    fscanf(stdin, "%" PRIu64 ",%f\n", &time, &value);

    fwrite(&time, sizeof(time), 1, stdout);
    fwrite(&value, sizeof(value), 1, stdout);
  }

  return 0;
}

int execute_to_csv(session_t *session, client_configuration_t *config) {
  fprintf(stderr, "converting to string\n");
  int points_size = 6553600;
  data_point_t *points = (data_point_t *)sdb_alloc(sizeof(data_point_t) * points_size);

  while (!feof(stdin)) {
    size_t read = fread(points, sizeof(data_point_t), (size_t)points_size, stdin);

    for (int i = 0; i < read; i++) {
      printf("%" PRIu64 ",%f\n", points[i].time, points[i].value);
    }
  }

  sdb_free(points);
  return 0;
}
