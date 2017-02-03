//
// Created by Pawel Burzynski on 01/02/2017.
//

#include <cstdlib>
#include <uv.h>
#include "uv-server.h"
#include <src/fatal-exception.h>
#include <src/utils/common.h>

namespace apollo {

void on_handle_close(uv_handle_t *handle) {
  free(handle);
}

void on_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  buf->base = (char *)calloc(suggested_size, 1);
  buf->len = suggested_size;
}

void on_data_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
  UvServer *_this = (UvServer *)client->data;
  UvServer::client_info_t *info = _this->clients[(uv_tcp_t *)client];

  if (nread < 0) {
    _this->log->Info("Read error, client id: " + std::to_string(info->id));
    _this->RemoveClient((uv_tcp_t *)client);
  } else if (nread == UV_EOF) {
    _this->log->Debug("Client disconnected: " + std::to_string(info->id));
    _this->RemoveClient((uv_tcp_t *)client);
  } else if (nread > 0) {
    while (nread + info->buffer_position > info->buffer_size) {
      info->buffer = (uint8_t *)realloc(info->buffer, info->buffer_size + A_SERVER_BUFFER_SIZE);
      info->buffer_size += A_SERVER_BUFFER_SIZE;
    }

    memcpy(info->buffer + info->buffer_position, buf->base, nread);
    info->buffer_position += nread;

    int offset = 0;
    int total_processed = 0;

    while (offset + sizeof(packet_type_t) <= info->buffer_position) {
      data_packet_t *packet = (data_packet_t *)(info->buffer + offset);

      if (offset + packet->total_length <= info->buffer_position) {
        _this->HandlePacket(packet);
        total_processed += packet->total_length;
      }

      offset += packet->total_length;
    }

    if (total_processed > 0) {
      size_t remaining = info->buffer_position - total_processed;
      size_t new_size = remaining + A_SERVER_BUFFER_SIZE;
      uint8_t *new_buffer = (uint8_t *)calloc(new_size, 1);

      memcpy(new_buffer, info->buffer + total_processed, remaining);
      free(info->buffer);

      info->buffer = new_buffer;
      info->buffer_size = new_size;
      info->buffer_position = 0;
    }
  }

  if (buf->base != nullptr && buf->len != 0) {
    free(buf->base);
  }
}

void on_new_connection(uv_stream_t *server, int status) {
  UvServer *_this = (UvServer *)server->data;

  if (status < 0) {
    _this->log->Info("New connection error: " + std::string(uv_strerror(status)));
    return;
  }

  _this->log->Debug("Client connected");
  uv_tcp_t *client = (uv_tcp_t *)calloc(1, sizeof(uv_tcp_t));
  uv_tcp_init(&_this->event_loop, client);

  if (uv_accept(server, (uv_stream_t *)client) == 0) {
    _this->RegisterClient(client);
    uv_read_start((uv_stream_t *)client, on_alloc_cb, on_data_read);
  } else {
    uv_close((uv_handle_t *)client, on_handle_close);
  }
}

UvServer::UvServer(int port, int backlog, std::vector<ClientHandler *> handlers, Log *log) {
  this->port = port;
  this->backlog = backlog;
  this->log = log;
  this->handlers = handlers;
  this->current_client_id = 0;

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

void UvServer::RegisterClient(uv_tcp_t *client) {
  client->data = this;

  client_info_t *info = (client_info_t *)calloc(1, sizeof(client_info_t));
  info->buffer = (uint8_t *)calloc(A_SERVER_BUFFER_SIZE, 1);
  info->buffer_size = A_SERVER_BUFFER_SIZE;
  info->buffer_position = 0;
  info->id = this->current_client_id++;

  this->clients[client] = info;
}

void UvServer::RemoveClient(uv_tcp_t *client) {
  if (this->clients.find(client) == this->clients.end()) {
    throw FatalException("Trying to free client that is not registered");
  }

  client_info_t *info = this->clients[client];
  this->clients.erase(client);

  uv_close((uv_handle_t *)client, on_handle_close);
  free(info->buffer);
  free(info);
}

void UvServer::HandlePacket(data_packet_t *packet) {
  for (auto handler: this->handlers) {
    // TODO: Responder
    handler->ProcessPacket(packet, nullptr);
  }
}

}