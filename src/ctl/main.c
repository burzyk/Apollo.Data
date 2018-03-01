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
  char command[SDB_STR_MAX_LEN];
  char hostname[SDB_STR_MAX_LEN];
  int port;
  series_id_t series_id;
  timestamp_t begin;
  timestamp_t end;
  uint32_t point_size;
} client_configuration_t;

void print_usage();
int client_configuration_load(int argc, char **argv, client_configuration_t *config);
int execute_command(session_t *session, client_configuration_t *config);
int execute_write(session_t *session, client_configuration_t *config);
int execute_read(session_t *session, client_configuration_t *config);
int execute_truncate(session_t *session, client_configuration_t *config);
int execute_get_latest(session_t *session, client_configuration_t *config);
int execute_from_csv(client_configuration_t *config);
int execute_to_csv(client_configuration_t *config);

int main(int argc, char *argv[]) {
  client_configuration_t config = {};
  strcpy(config.hostname, "localhost");
  config.port = 8487;
  config.series_id = 0;
  config.begin = 0;
  config.end = 0;
  config.point_size = 12;

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
  printf("    --size, -x:       Data point size\n");
  printf("                      Default value: 12 (8 bytes timestamp, 4 bytes payload)\n");
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
        {"end", required_argument, 0, 'e'},
        {"width", required_argument, 0, 'w'},
        {"size", required_argument, 0, 'x'}
    };

    int c = getopt_long(argc, argv, "c:m:p:s:b:e:x:", long_options, &option_index);
    if (c == -1) {
      return 0;
    }

    switch (c) {
      case 'c':strncpy(config->command, optarg, SDB_STR_MAX_LEN);
        break;
      case 'h':strncpy(config->hostname, optarg, SDB_STR_MAX_LEN);
        break;
      case 'p':config->port = (int)strtol(optarg, NULL, 0);
        break;
      case 's':config->series_id = (series_id_t)strtol(optarg, NULL, 0);
        break;
      case 'b':config->begin = strtoull(optarg, NULL, 0);
        break;
      case 'e':config->end = strtoull(optarg, NULL, 0);
        break;
      case 'x':config->point_size = (uint32_t)strtoul(optarg, NULL, 0);
        break;
      default:return -1;
    }
  }
}

int execute_command(session_t *session, client_configuration_t *config) {
  if (!strncmp(SDB_CLIENT_CMD_WRITE, config->command, SDB_STR_MAX_LEN)) {
    return execute_write(session, config);

  } else if (!strncmp(SDB_CLIENT_CMD_READ, config->command, SDB_STR_MAX_LEN)) {
    return execute_read(session, config);

  } else if (!strncmp(SDB_CLIENT_CMD_TRUNCATE, config->command, SDB_STR_MAX_LEN)) {
    return execute_truncate(session, config);

  } else if (!strncmp(SDB_CLIENT_CMD_LATEST, config->command, SDB_STR_MAX_LEN)) {
    return execute_get_latest(session, config);

  } else if (!strncmp(SDB_CLIENT_CMD_TO_CSV, config->command, SDB_STR_MAX_LEN)) {
    return execute_to_csv(config);

  } else if (!strncmp(SDB_CLIENT_CMD_FROM_CSV, config->command, SDB_STR_MAX_LEN)) {
    return execute_from_csv(config);

  } else if (!strncmp("", config->command, SDB_STR_MAX_LEN)) {
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
  data_point_t *points = (data_point_t *)sdb_alloc(config->point_size * points_size);

  while (!feof(stdin)) {
    size_t read = fread(points, config->point_size, (size_t)points_size, stdin);

    if (read > 0) {
      points_list_t p = {
          .content = points,
          .count = read,
          .point_size = config->point_size
      };
      if (session_write(session, config->series_id, &p)) {
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
        session->read_response->point_size,
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

  if (session_read_latest(session, config->series_id)) {
    fprintf(stderr, "failed to get latest data point\n");
    return -1;
  }

  while (session_read_next(session)) {
    fwrite(
        session->read_response->points,
        session->read_response->point_size,
        (size_t)session->read_response->points_count,
        stdout);
  }

  if (session->read_response->points_count == 0) {
    fprintf(stderr, "time series empty\n");
  }

  return 0;
}

int execute_from_csv(client_configuration_t *config) {
  if (config->point_size != 12) {
    fprintf(stderr, "only float data points are supported");
    return -1;
  }

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

int execute_to_csv(client_configuration_t *config) {
  if (config->point_size != 12) {
    fprintf(stderr, "only float data points are supported");
    return -1;
  }

  fprintf(stderr, "converting to csv\n");

  int points_size = 6553600;
  data_point_t *points = (data_point_t *)sdb_alloc(config->point_size * points_size);

  while (!feof(stdin)) {
    size_t read = fread(points, config->point_size, (size_t)points_size, stdin);

    points_list_t list = {
        .point_size = config->point_size,
        .content = points,
        .count = read
    };

    data_point_t *curr = list.content;
    data_point_t *end = points_list_end(&list);

    while (curr != end) {
      printf("%" PRIu64 ",%f\n", curr->time, *(float *)curr->value);
      curr = data_point_next(&list, curr);
    }
  }

  sdb_free(points);
  return 0;
}
