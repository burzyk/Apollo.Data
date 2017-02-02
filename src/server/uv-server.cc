//
// Created by Pawel Burzynski on 01/02/2017.
//

#include <cstdlib>
#include <uv.h>
#include "uv-server.h"
#include <src/fatal-exception.h>

namespace apollo {

void on_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  buf->base = (char *)malloc(suggested_size);
  buf->len = suggested_size;
}

void on_data_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
  UvServer *_this = (UvServer *)client->data;

  if (nread < 0) {
    _this->log->Info("Read error");
    _this->RemoveClient((uv_tcp_t *)client);
  } else if (nread == UV_EOF) {
    // TODO: end and cleanun
  } else if (nread > 0) {
    // data available
  }
}

void on_new_connection(uv_stream_t *server, int status) {
  UvServer *_this = (UvServer *)server->data;

  if (status < 0) {
    _this->log->Info("New connection error: " + std::string(uv_strerror(status)));
    return;
  }

  _this->log->Debug("Client connected");

  uv_tcp_t *client = _this->AddClient();
  uv_tcp_init(&_this->event_loop, client);

  if (uv_accept(server, (uv_stream_t *)client) == 0) {
    uv_read_start((uv_stream_t *)client, on_alloc_cb, on_data_read);
  } else {
    _this->RemoveClient(client);
  }
}

UvServer::UvServer(int port, int backlog, std::vector<ClientHandler *> handlers, Log *log) {
  this->port = port;
  this->backlog = backlog;
  this->log = log;
  this->handlers = handlers;

  uv_loop_init(&this->event_loop);
  uv_tcp_init(&this->event_loop, &this->server);

  this->server.data = this;
}

UvServer::~UvServer() {
  // TODO: close loop;
}

void UvServer::Listen() {
  this->log->Info("Starting listening on port: " + std::to_string(this->port));

  struct sockaddr_in address = {0};
  uv_ip4_addr("0.0.0.0", this->port, &address);

  uv_tcp_bind(&this->server, (const struct sockaddr *)&address, 0);

  if (uv_listen((uv_stream_t *)&this->server, this->backlog, on_new_connection) != 0) {
    throw FatalException("Unable to listen");
  }

  uv_run(&this->event_loop, UV_RUN_DEFAULT);
  this->log->Info("Server loop exited");
}

uv_tcp_t *UvServer::AddClient() {
  uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
  client->data = this;
  this->clients.push_back(client);

  return client;
}

void UvServer::RemoveClient(uv_tcp_t *client) {
  this->clients.remove(client);
  uv_close((uv_handle_t *)client, NULL);
  free(client);
}

}