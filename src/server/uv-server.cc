//
// Created by Pawel Burzynski on 01/02/2017.
//

#include <cstdlib>
#include <uv.h>
#include "uv-server.h"
#include <src/fatal-exception.h>

namespace apollo {

void on_new_connection(uv_stream_t *server, int status) {
  server_info_t *info = (server_info_t *)server->data;

  if (status < 0) {
    info->log->Info("New connection error: " + std::string(uv_strerror(status)));
    return;
  }

  info->log->Debug("Client connected");

  uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
  info->clients.push_back(client);

  uv_tcp_init(&info->event_loop, client);

  if (uv_accept(server, (uv_stream_t *)client) == 0) {

    // TODO: read stuff
    // uv_write(&write_req, (uv_stream_t *)&client, &send_buff, 1, on_write);

  } else {
    info->clients.remove(client);
    uv_close((uv_handle_t *)client, NULL);
    free(client);
  }
}

UvServer::UvServer(int port, int backlog, std::vector<ClientHandler *> handlers, Log *log) {
  this->port = port;
  this->backlog = backlog;

  uv_loop_init(&this->info.event_loop);
  uv_tcp_init(&this->info.event_loop, &this->info.server);

  this->info.log = log;
  this->info.handlers = handlers;
  this->info.log = log;
  this->info.server.data = &this->info;
}

UvServer::~UvServer() {
  // TODO: close loop;
}

void UvServer::Listen() {
  this->info.log->Info("Starting listening on port: " + std::to_string(this->port));

  struct sockaddr_in addr = {0};
  uv_ip4_addr("0.0.0.0", this->port, &addr);

  uv_tcp_bind(&this->info.server, (const struct sockaddr *)&addr, 0);

  if (uv_listen((uv_stream_t *)&this->info.server, this->backlog, on_new_connection) != 0) {
    throw FatalException("Unable to listen");
  }

  uv_run(&this->info.event_loop, UV_RUN_DEFAULT);
  this->info.log->Info("Server loop exited");
}

}