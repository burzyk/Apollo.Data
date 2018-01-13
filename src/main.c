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
// Created by Pawel Burzynski on 15/04/2017.
//

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <memory.h>
#include <signal.h>
#include <inttypes.h>
#include <libdill.h>

#include "src/storage/database.h"
#include "src/server/server.h"
#include "src/utils/diagnostics.h"
#include "src/utils/memory.h"

#ifndef SDB_VERSION
#define SDB_VERSION "0.0.1"
#endif

#ifndef SDB_BUILD
#define SDB_BUILD "<COMMIT_ID>"
#endif

#define SDB_CONFIG_DEFAULT_DIRECTORY  "/var/lib/shakadb"
#define SDB_CONFIG_DEFAULT_PORT 8487
#define SDB_CONFIG_DEFAULT_SOFT_LIMIT 1500000000
#define SDB_CONFIG_DEFAULT_HARD_LIMIT 2000000000

typedef struct sdb_configuration_s {
  int log_verbose;
  char database_directory[SDB_FILE_MAX_LEN];
  int database_points_per_chunk;
  int server_port;
  uint64_t cache_soft_limit;
  uint64_t cache_hard_limit;
} sdb_configuration_t;

sdb_server_t *g_server;

void sdb_master_routine(sdb_configuration_t *data);
void sdb_print_banner(sdb_configuration_t *config);
void sdb_print_usage();
int sdb_configuration_parse(sdb_configuration_t *config, int argc, char *argv[]);
void sdb_control_signal_handler(int sig);

int main(int argc, char *argv[]) {

  sdb_configuration_t config = {};

  if (sdb_configuration_parse(&config, argc, argv)) {
    sdb_print_usage();
    return -1;
  }

  sdb_log_init(config.log_verbose);

  sdb_print_banner(&config);

  signal(SIGUSR1, sdb_control_signal_handler);
  signal(SIGTERM, sdb_control_signal_handler);
  signal(SIGINT, sdb_control_signal_handler);

  sdb_master_routine(&config);

  sdb_log_info("========== ShakaDB Stopped  ==========");
  sdb_log_close();

  return 0;
}

void sdb_control_signal_handler(int sig) {
  if (g_server != NULL) {
    sdb_server_stop(g_server);
  }
}

void sdb_master_routine(sdb_configuration_t *config) {

  sdb_log_info("initializing database ...");
  sdb_database_t *db = sdb_database_create(
      config->database_directory,
      config->database_points_per_chunk,
      SDB_DATA_SERIES_MAX,
      config->cache_soft_limit,
      config->cache_hard_limit);

  sdb_log_info("initializing server ...");
  g_server = sdb_server_create(config->server_port, db);

  sdb_log_info("initialization complete");

  sdb_server_run(g_server);

  sdb_log_info("interrupt received");

  sdb_log_info("closing server ...");
  sdb_server_destroy(g_server);

  sdb_log_info("closing database ...");
  sdb_database_destroy(db);
}

int sdb_configuration_parse(sdb_configuration_t *config, int argc, char *argv[]) {
  config->server_port = SDB_CONFIG_DEFAULT_PORT;
  strncpy(config->database_directory, SDB_CONFIG_DEFAULT_DIRECTORY, SDB_FILE_MAX_LEN);
  config->log_verbose = 0;
  config->database_points_per_chunk = 10000;
  config->cache_soft_limit = SDB_CONFIG_DEFAULT_SOFT_LIMIT;
  config->cache_hard_limit = SDB_CONFIG_DEFAULT_HARD_LIMIT;

  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
        {"port", required_argument, 0, 'p'},
        {"directory", required_argument, 0, 'd'},
        {"soft-limit", required_argument, 0, 's'},
        {"hard-limit", required_argument, 0, 'x'},
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'}
    };

    int c = getopt_long(argc, argv, "vhp:d:x:s:", long_options, &option_index);
    if (c == -1) {
      config->cache_soft_limit = sdb_minl(config->cache_soft_limit, config->cache_hard_limit);
      return 0;
    }

    switch (c) {
      case 'p': config->server_port = atoi(optarg);
        break;
      case 'd':strncpy(config->database_directory, optarg, SDB_FILE_MAX_LEN);
        break;
      case 'h':sdb_print_usage();
        exit(0);
      case 'v':config->log_verbose = 1;
        break;
      case 's':sscanf(optarg, "%" PRIu64, &config->cache_soft_limit);
        break;
      case 'x':sscanf(optarg, "%" PRIu64, &config->cache_hard_limit);
        break;
      default:return -1;
    }
  }
}

void sdb_print_banner(sdb_configuration_t *config) {

  sdb_log_info("========== Starting ShakaDB ==========");
  sdb_log_info("");
  sdb_log_info("    Version:   " SDB_VERSION);
  sdb_log_info("    Build:     " SDB_BUILD);
  sdb_log_info("");
  sdb_log_info("    directory:   %s", config->database_directory);
  sdb_log_info("    port:        %d", config->server_port);
  sdb_log_info("    soft limit:  %" PRIu64 " bytes", config->cache_soft_limit);
  sdb_log_info("    hard limit:  %" PRIu64 " bytes", config->cache_hard_limit);
  sdb_log_info("");
}

void sdb_print_usage() {
  printf("\n");
  printf("ShakaDB - time series database\n");
  printf("\n");
  printf("    Version:\t" SDB_VERSION "\n");
  printf("    Build:\t" SDB_BUILD "\n");
  printf("\n");
  printf("Usage: shakadb <options>\n");
  printf("Options:\n");
  printf("\n");
  printf("    --help, -h:       displays this information\n");
  printf("    --port, -p:       port to listen on\n");
  printf("                      default value: %d\n", SDB_CONFIG_DEFAULT_PORT);
  printf("    --directory, -d:  directory where database will be created\n");
  printf("                      default value: %s\n", SDB_CONFIG_DEFAULT_DIRECTORY);
  printf("    --verbose, -v:    logs debug information\n");
  printf("    --soft-limit, -s: specifies the soft cache memory usage limit, in bytes\n");
  printf("                      default value: %d\n", SDB_CONFIG_DEFAULT_SOFT_LIMIT);
  printf("    --hard-limit, -x: specifies the hard cache memory usage limit, in bytes\n");
  printf("                      default value: %d\n", SDB_CONFIG_DEFAULT_HARD_LIMIT);
  printf("\n");
  printf("For more info visit: http://shakadb.com/getting-started\n");
  printf("\n");
}
