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

#include "src/utils/threading.h"
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

#define SDB_CONFIG_DEFAULT_DIRECTORY  "/usr/local/shakadb/data"
#define SDB_CONFIG_DEFAULT_PORT 8487
#define SDB_CONFIG_DEFAULT_LOG  "stdout"

typedef struct sdb_control_info_s {
  sdb_rwlock_t *_lock;
  volatile int _is_running;
} sdb_control_info_t;

sdb_control_info_t *sdb_control_info_create();
void sdb_control_info_destroy(sdb_control_info_t *info);
int sdb_control_info_check_running(sdb_control_info_t *info);
void sdb_control_info_signal_stop(sdb_control_info_t *info);

typedef struct sdb_configuration_s {
  char log_file[SDB_FILE_MAX_LEN];
  int log_verbose;
  char database_directory[SDB_FILE_MAX_LEN];
  int database_points_per_chunk;
  int server_port;
  int server_backlog;
  int server_max_clients;
  int server_points_per_packet;
} sdb_configuration_t;

void *sdb_master_thread_routine(void *data);
void sdb_print_usage();
int sdb_configuration_parse(sdb_configuration_t *config, int argc, char *argv[]);
void sdb_control_signal_handler(int sig);

sdb_control_info_t *g_control = NULL;

int main(int argc, char *argv[]) {
  sdb_configuration_t config = {};

  if (sdb_configuration_parse(&config, argc, argv)) {
    sdb_print_usage();
    return -1;
  }

  sdb_log_init(config.log_file, config.log_verbose);
  sdb_log_info("========== Starting ShakaDB ==========");
  sdb_log_info("");
  sdb_log_info("    Version:   " SDB_VERSION);
  sdb_log_info("    Build:     " SDB_BUILD);
  sdb_log_info("");
  sdb_log_info("    directory: %s", config.database_directory);
  sdb_log_info("    port:      %d", config.server_port);
  sdb_log_info("    log:       %s", config.log_file);
  sdb_log_info("");

  g_control = sdb_control_info_create();
  signal(SIGUSR1, sdb_control_signal_handler);
  signal(SIGTERM, sdb_control_signal_handler);
  signal(SIGINT, sdb_control_signal_handler);

  sdb_thread_t *master_thread = sdb_thread_start(sdb_master_thread_routine, &config);
  sdb_thread_join_and_destroy(master_thread);

  sdb_control_info_destroy(g_control);

  sdb_log_info("========== ShakaDB Stopped  ==========");
  sdb_log_close();

  return 0;
}

void sdb_control_signal_handler(int sig) {
  sdb_control_info_signal_stop(g_control);
}

void *sdb_master_thread_routine(void *data) {
  sdb_configuration_t *config = (sdb_configuration_t *)data;

  sdb_log_info("initializing database ...");
  sdb_database_t *db = sdb_database_create(config->database_directory, config->database_points_per_chunk);

  sdb_log_info("initializing server ...");
  sdb_server_t *server = sdb_server_create(
      config->server_port,
      config->server_backlog,
      config->server_max_clients,
      config->server_points_per_packet,
      db);

  sdb_log_info("initialization complete");

  while (sdb_control_info_check_running(g_control)) {
    sdb_thread_sleep(200);
  }

  sdb_log_info("interrupt received");

  sdb_log_info("closing server ...");
  sdb_server_destroy(server);

  sdb_log_info("closing database ...");
  sdb_database_destroy(db);

  return NULL;
}

int sdb_configuration_parse(sdb_configuration_t *config, int argc, char *argv[]) {
  config->server_port = SDB_CONFIG_DEFAULT_PORT;
  strncpy(config->database_directory, SDB_CONFIG_DEFAULT_DIRECTORY, SDB_FILE_MAX_LEN);
  strncpy(config->log_file, SDB_CONFIG_DEFAULT_LOG, SDB_FILE_MAX_LEN);
  config->log_verbose = 0;
  config->server_backlog = 20;
  config->server_max_clients = 10;
  config->server_points_per_packet = 655360;
  config->database_points_per_chunk = 10000;

  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
        {"port", required_argument, 0, 'p'},
        {"directory", required_argument, 0, 'd'},
        {"log", required_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'}
    };

    int c = getopt_long(argc, argv, "vhp:d:l:", long_options, &option_index);
    if (c == -1) {
      return 0;
    }

    switch (c) {
      case 'p': config->server_port = atoi(optarg);
        break;
      case 'd':strncpy(config->database_directory, optarg, SDB_FILE_MAX_LEN);
        break;
      case 'l':strncpy(config->log_file, optarg, SDB_FILE_MAX_LEN);
        break;
      case 'h':sdb_print_usage();
        exit(0);
      case 'v':config->log_verbose = 1;
        break;
      default:return -1;
    }
  }
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
  printf("    --log, -l:        log file name. If 'stdout' is specified the application\n");
  printf("                      will write all logs to standard output\n");
  printf("                      default value: %s\n", SDB_CONFIG_DEFAULT_LOG);
  printf("    --verbose, -v:    logs debug information\n");
  printf("\n");
  printf("For more info visit: http://shakadb.com/getting-started\n");
  printf("\n");
}

sdb_control_info_t *sdb_control_info_create() {
  sdb_control_info_t *info = (sdb_control_info_t *)sdb_alloc(sizeof(sdb_control_info_t));
  info->_is_running = 1;
  info->_lock = sdb_rwlock_create();

  return info;
}

void sdb_control_info_destroy(sdb_control_info_t *info) {
  sdb_rwlock_destroy(info->_lock);
  sdb_free(info);
}

int sdb_control_info_check_running(sdb_control_info_t *info) {
  sdb_rwlock_rdlock(info->_lock);
  int status = info->_is_running;
  sdb_rwlock_unlock(info->_lock);

  return status;
}

void sdb_control_info_signal_stop(sdb_control_info_t *info) {
  sdb_rwlock_wrlock(info->_lock);
  info->_is_running = 0;
  sdb_rwlock_unlock(info->_lock);
}
