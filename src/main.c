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

#include "src/network/client-handler.h"
#include "src/diagnostics.h"

#ifndef SDB_VERSION
#define SDB_VERSION "0.0.1"
#endif

#ifndef SDB_BUILD
#define SDB_BUILD "<COMMIT_ID>"
#endif

#define SDB_CONFIG_DEFAULT_DIRECTORY  "/var/lib/shakadb"
#define SDB_CONFIG_DEFAULT_PORT 8487

typedef struct configuration_s {
  int log_verbose;
  char database_directory[SDB_STR_MAX_LEN];
  int server_port;
} configuration_t;

server_t *g_server;

void master_routine(configuration_t *config);
void print_banner(configuration_t *config);
void print_usage();
int configuration_parse(configuration_t *config, int argc, char **argv);
void control_signal_handler(int sig);
int on_message_received(client_t *client, uint8_t *data, uint32_t size, void *context);

int main(int argc, char *argv[]) {
  configuration_t config = {};

  if (configuration_parse(&config, argc, argv)) {
    print_usage();
    return -1;
  }

  log_init(config.log_verbose);

  print_banner(&config);

  signal(SIGUSR1, control_signal_handler);
  signal(SIGTERM, control_signal_handler);
  signal(SIGINT, control_signal_handler);

  master_routine(&config);

  log_info("========== ShakaDB Stopped  ==========");
  log_close();

  return 0;
}

void control_signal_handler(int sig) {
  if (g_server != NULL) {
    server_stop(g_server);
  }
}

void master_routine(configuration_t *config) {

  log_info("initializing database ...");
  database_t *db = database_create(config->database_directory, SDB_DATA_SERIES_MAX);

  log_info("initializing server ...");
  client_handler_t *handler = client_handler_create(db);
  g_server = server_create(config->server_port, on_message_received, handler);

  log_info("initialization complete");

  server_run(g_server);

  log_info("interrupt received");

  log_info("closing server ...");
  server_destroy(g_server);
  client_handler_destroy(handler);

  log_info("closing database ...");
  database_destroy(db);
}

int on_message_received(client_t *client, uint8_t *data, uint32_t size, void *context) {
  client_handler_t *handler = (client_handler_t *)context;
  return client_handler_process_message(client, data, size, handler);
}

int configuration_parse(configuration_t *config, int argc, char **argv) {
  config->server_port = SDB_CONFIG_DEFAULT_PORT;
  strncpy(config->database_directory, SDB_CONFIG_DEFAULT_DIRECTORY, SDB_STR_MAX_LEN);
  config->log_verbose = 0;

  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
        {"port", required_argument, 0, 'p'},
        {"directory", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'}
    };

    int c = getopt_long(argc, argv, "vhp:d:", long_options, &option_index);
    if (c == -1) {
      return 0;
    }

    switch (c) {
      case 'p': config->server_port = atoi(optarg);
        break;
      case 'd':strncpy(config->database_directory, optarg, SDB_STR_MAX_LEN);
        break;
      case 'h':print_usage();
        exit(0);
      case 'v':config->log_verbose = 1;
        break;
      default:return -1;
    }
  }
}

void print_banner(configuration_t *config) {

  log_info("          _           _             _ _          ");
  log_info("      ___| |__   __ _| | ____ _  __| | |__       ");
  log_info("     / __| '_ \\ / _` | |/ / _` |/ _` | '_ \\      ");
  log_info("     \\__ \\ | | | (_| |   < (_| | (_| | |_) |     ");
  log_info("     |___/_| |_|\\__,_|_|\\_\\__,_|\\__,_|_.__/      ");
  log_info("                                                 ");
  log_info("    Version:   " SDB_VERSION);
  log_info("    Build:     " SDB_BUILD);
  log_info("");
  log_info("    directory:   %s", config->database_directory);
  log_info("    port:        %d", config->server_port);
  log_info("");
}

void print_usage() {
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
  printf("\n");
  printf("For more info visit: http://shakadb.com/getting-started\n");
  printf("\n");
}
