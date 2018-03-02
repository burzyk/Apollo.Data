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

#define SDB_TYPE_FLOAT  0x01
#define SDB_TYPE_DOUBLE 0x02
#define SDB_TYPE_INT32  0x03
#define SDB_TYPE_INT64  0x04
#define SDB_TYPE_STRING 0x05

#define SDB_FORMAT_TIMESTAMP  "%" PRIu64 ","
#define SDB_FORMAT_INT32      SDB_FORMAT_TIMESTAMP "%" PRIi32
#define SDB_FORMAT_INT64      SDB_FORMAT_TIMESTAMP "%" PRIi64
#define SDB_FORMAT_FLOAT      SDB_FORMAT_TIMESTAMP "%f"
#define SDB_FORMAT_DOUBLE     SDB_FORMAT_TIMESTAMP "%lf"

typedef struct client_configuration_s {
  char *command;
  char *hostname;
  int port;
  series_id_t series_id;
  timestamp_t begin;
  timestamp_t end;
  uint32_t point_size;
  uint8_t data_type;
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
uint8_t parse_data_type(const char *type);

int main(int argc, char *argv[]) {
  client_configuration_t config = {};
  config.command = NULL;
  config.hostname = "localhost";
  config.port = 8487;
  config.series_id = 0;
  config.begin = 0;
  config.end = 0;
  config.point_size = 0;
  config.data_type = 0;

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
  printf("    version:\t" SDB_VERSION "\n");
  printf("    build:\t" SDB_BUILD "\n");
  printf("\n");
  printf("Usage: shakadbctl <command> [options]\n");
  printf("\n");
  printf("    global parameters:\n");
  printf("\n");
  printf("    --hostname, -h:   shakadb server address, can be IP or hostname\n");
  printf("                      default value: 'localhost'\n");
  printf("    --port, -p:       shakadb server port\n");
  printf("                      default value: 8487\n");
  printf("\n");
  printf("    available commands:\n");
  printf("\n");
  printf("    write        writes the binary stream from STDIN\n");
  printf("                 to the time series\n");
  printf("                     --series, -s    time series id\n");
  printf("                     --size, -x      data point value size\n");
  printf("                                     can be derived from the type parameter\n");
  printf("                     --type, -t      optional type of the data point\n");
  printf("                                     available values: int32, int64, string, float, double\n");
  printf("\n");
  printf("    read         reads the time series and outputs\n");
  printf("                 the binary stream to STDOUT\n");
  printf("                     --series, -s    time series id\n");
  printf("                     --begin, -b     begin for reading, inclusive\n");
  printf("                     --end, -e       end for reading, exclusive\n");
  printf("\n");
  printf("    truncate     removes all data from the given time series\n");
  printf("                     --series, -s    time series id\n");
  printf("\n");
  printf("    latest       reads the latest value from the time series\n");
  printf("                 and outputs in binary format to STDOUT\n");
  printf("                     --series, -s    time series id\n");
  printf("\n");
  printf("    to_csv       converts the binary stream of data points from STDIN\n");
  printf("                 into human readable CSV format\n");
  printf("                     --size, -x      data point value size\n");
  printf("                                     required only for strings\n");
  printf("                     --type, -t      type of the data point\n");
  printf("                                     available values: int32, int64, string, float, double\n");
  printf("\n");
  printf("    from_csv     reads data from the STDIN in CSV format and converts to binary\n");
  printf("                 writing to STDOUT\n");
  printf("                     --size, -x      data point value size\n");
  printf("                                     required only for strings\n");
  printf("                     --type, -t      type of the data point\n");
  printf("                                     available values: int32, int64, string, float, double\n");
  printf("\n");
  printf("\n");
  printf("For more info visit: http://shakadb.com/getting-started\n");
  printf("\n");
}

int client_configuration_load(int argc, char **argv, client_configuration_t *config) {
  if (argc < 2) {
    return -1;
  }

  config->command = argv[1];

  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
        {"hostname", required_argument, 0, 'h'},
        {"port", required_argument, 0, 'p'},
        {"series", required_argument, 0, 's'},
        {"begin", required_argument, 0, 'b'},
        {"end", required_argument, 0, 'e'},
        {"size", required_argument, 0, 'x'},
        {"type", required_argument, 0, 't'}
    };

    int c = getopt_long(argc, argv, "h:p:s:b:e:x:t:", long_options, &option_index);
    if (c == -1) {
      if (config->data_type != 0) {
        switch (config->data_type) {
          case SDB_TYPE_DOUBLE:
          case SDB_TYPE_INT64:config->point_size = 8 + sizeof(timestamp_t);
            break;
          case SDB_TYPE_FLOAT:
          case SDB_TYPE_INT32:config->point_size = 4 + sizeof(timestamp_t);
            break;
          case SDB_TYPE_STRING:
            if (config->point_size == 0) {
              fprintf(stderr, "string has to have the size specified\n");
              return -1;
            }
            break;
          default:fprintf(stderr, "invalid data type\n");
            return -1;
        }
      }

      return 0;
    }

    switch (c) {
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
      case 'x':config->point_size = (uint32_t)strtoul(optarg, NULL, 0) + sizeof(timestamp_t);
        break;
      case 't':config->data_type = parse_data_type(optarg);
        break;
      default:return -1;
    }
  }
}

uint8_t parse_data_type(const char *type) {
  if (!strncmp("string", type, SDB_STR_MAX_LEN)) {
    return SDB_TYPE_STRING;
  } else if (!strncmp("int32", type, SDB_STR_MAX_LEN)) {
    return SDB_TYPE_INT32;
  } else if (!strncmp("int64", type, SDB_STR_MAX_LEN)) {
    return SDB_TYPE_INT64;
  } else if (!strncmp("float", type, SDB_STR_MAX_LEN)) {
    return SDB_TYPE_FLOAT;
  } else if (!strncmp("double", type, SDB_STR_MAX_LEN)) {
    return SDB_TYPE_DOUBLE;
  }

  die("Unknown type");
  return 0;
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
  if (config->point_size == 0 || config->data_type == 0) {
    fprintf(stderr, "unknown point size/type\n");
    return -1;
  }

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
  if (config->point_size == 0 || config->data_type == 0) {
    fprintf(stderr, "unknown point size/type\n");
    return -1;
  }

  fprintf(stderr, "converting to binary\n");
  timestamp_t time;
  size_t value_size = config->point_size - sizeof(timestamp_t);
  void *value = sdb_alloc(value_size);
  char *line = NULL;
  size_t line_len = 0;

  while (!feof(stdin)) {
    switch (config->data_type) {
      case SDB_TYPE_INT32: fscanf(stdin, SDB_FORMAT_INT32, &time, (int32_t *)value);
        break;
      case SDB_TYPE_INT64: fscanf(stdin, SDB_FORMAT_INT64, &time, (int64_t *)value);
        break;
      case SDB_TYPE_FLOAT: fscanf(stdin, SDB_FORMAT_FLOAT, &time, (float *)value);
        break;
      case SDB_TYPE_DOUBLE: fscanf(stdin, SDB_FORMAT_DOUBLE, &time, (double *)value);
        break;
      case SDB_TYPE_STRING:fscanf(stdin, SDB_FORMAT_TIMESTAMP, &time);
        if (feof(stdin)) {
          return 0;
        }

        // this gets the rest of the line
        getline(&line, &line_len, stdin);

        if (line_len > value_size) {
          die("line too long");
        }

        memset(value, 0, value_size);

        // ignore the trailing new-line character
        strncpy(value, line, strlen(line) - 1);
        break;
      default:die("Unknown type");
    }

    fwrite(&time, sizeof(time), 1, stdout);
    fwrite(value, value_size, 1, stdout);
  }

  if (line != NULL) {
    sdb_free(line);
  }

  sdb_free(value);

  return 0;
}

int execute_to_csv(client_configuration_t *config) {
  if (config->point_size == 0 || config->data_type == 0) {
    fprintf(stderr, "unknown point size/type\n");
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
      switch (config->data_type) {
        case SDB_TYPE_INT32: printf(SDB_FORMAT_INT32 "\n", curr->time, *(int32_t *)curr->value);
          break;
        case SDB_TYPE_INT64: printf(SDB_FORMAT_INT64 "\n", curr->time, *(int64_t *)curr->value);
          break;
        case SDB_TYPE_FLOAT: printf(SDB_FORMAT_FLOAT "\n", curr->time, *(float *)curr->value);
          break;
        case SDB_TYPE_DOUBLE: printf(SDB_FORMAT_DOUBLE "\n", curr->time, *(double *)curr->value);
          break;
        case SDB_TYPE_STRING: printf(SDB_FORMAT_TIMESTAMP "%s\n", curr->time, curr->value);
          break;
        default:die("Unknown type");
      }

      curr = data_point_next(&list, curr);
    }
  }

  sdb_free(points);
  return 0;
}
