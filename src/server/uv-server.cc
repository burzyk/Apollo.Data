//
// Created by Pawel Burzynski on 01/02/2017.
//

#include <cstdlib>
#include <uv.h>
#include "uv-server.h"
#include "uv-common.h"
#include <src/fatal-exception.h>
#include <src/utils/common.h>
#include <src/utils/allocator.h>

namespace apollo {

UvServer::UvServer(int port, int backlog, Log *log) {
  this->port = port;
  this->backlog = backlog;
  this->log = log;
  this->is_running = false;

  uv_loop_init(&this->event_loop);
  uv_tcp_init(&this->event_loop, &this->server);

  this->server.data = this;
}

void UvServer::Listen() {
  this->is_running = true;
  this->log->Info("Starting listening on port: " + std::to_string(this->port));

  struct sockaddr_in address = {0};
  uv_ip4_addr("0.0.0.0", this->port, &address);

  uv_tcp_bind(&this->server, (const struct sockaddr *)&address, 0);

  if (uv_listen((uv_stream_t *)&this->server, this->backlog, OnNewConnection) != 0) {
    throw FatalException("Unable to listen");
  }

  uv_run(&this->event_loop, UV_RUN_DEFAULT);
  uv_loop_close(&this->event_loop);

  this->CleanDisconnectedClients();
  this->log->Info("Server loop exited");
}

void UvServer::Close() {
  this->is_running = false;
  this->log->Info("Shutting down server");

  uv_async_t *shutdown = Allocator::New<uv_async_t>();
  uv_async_init(&this->event_loop, shutdown, OnServerClose);
  shutdown->data = this;
  uv_async_send(shutdown);
}

void UvServer::AddClientConnectedListener(Server::ClientConnectedListener *listener) {
  this->client_connected_listeners.push_back(listener);
}

void UvServer::OnNewConnection(uv_stream_t *server, int status) {
  UvServer *_this = (UvServer *)server->data;

  if (!_this->is_running) {
    return;
  }

  if (status < 0) {
    _this->log->Info("New connection error: " + std::string(uv_strerror(status)));
    return;
  }

  _this->CleanDisconnectedClients();
  UvServerClient *client = UvServerClient::Accept(server, &_this->event_loop);

  if (client != nullptr) {
    _this->clients.push_back(client);

    for (auto listener: _this->client_connected_listeners) {
      listener->OnClientConnected(_this, client);
    }
  }
}

void UvServer::OnServerClose(uv_async_t *handle) {
  UvServer *_this = (UvServer *)handle->data;

  for (auto client: _this->clients) {
    client->Close();
  }

  uv_close((uv_handle_t *)handle, UvCommon::OnHandleClose);
  uv_close((uv_handle_t *)&_this->server, nullptr);
}

void UvServer::CleanDisconnectedClients() {
  for (auto client = this->clients.begin(); client != this->clients.end();) {
    if ((*client)->IsRunning()) {
      client++;
    } else {
      delete *client;
      client = this->clients.erase(client);
    }
  }
}

}