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
// Created by Pawel Burzynski on 25/02/2017.
//

#include "src/server/web-server.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "src/utils/allocator.h"

namespace shakadb {

WebServer::WebServer(int port, int backlog, int max_clients) {
  this->port = port;
  this->backlog = backlog;
  this->max_clients = max_clients;
  this->is_running = false;
  this->master_socket = -1;
  this->next_client_id = 10;
  this->server_lock = sdb_mutex_create();
}

WebServer::~WebServer() {
  if (this->is_running) {
    this->Close();
  }

  sdb_mutex_destroy(this->server_lock);
}

void WebServer::Listen() {
  signal(SIGPIPE, SIG_IGN);

  if ((this->master_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    die("Unable to open main socket");
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(this->port);
  addr.sin_addr.s_addr = inet_addr("0.0.0.0");

  if (bind(this->master_socket, (struct sockaddr *)&addr, sizeof(sockaddr_in)) == -1) {
    die("Unable to bind to the socket");
  }

  if (listen(this->master_socket, this->backlog) == -1) {
    die("Unable to listen");
  }

  this->is_running = true;

  for (int i = 0; i < this->max_clients; i++) {
    sdb_thread_t *worker = sdb_thread_create();
    sdb_thread_start(worker, (sdb_thread_routine_t)WorkerRoutine, this);
    this->thread_pool.push_back(worker);

  }
}

void WebServer::WorkerRoutine(void *data) {
  WebServer *_this = (WebServer *)data;

  while (_this->is_running) {
    int client_socket;

    if ((client_socket = sdb_socket_accept(_this->master_socket)) != -1) {
      int client_id = _this->AllocateClient(client_socket);
      sdb_packet_t *packet = NULL;

      while ((packet = sdb_packet_receive(client_socket)) != NULL) {
        for (auto listener : _this->listeners) {
          listener->OnPacketReceived(client_id, packet);
        }

        sdb_packet_destroy(packet);
      }

      _this->CloseClient(client_id);
    }
  }
}

void WebServer::Close() {
  sdb_mutex_lock(this->server_lock);
  this->is_running = 0;

  for (auto client : this->clients) {
    sdb_socket_close(client.second->socket);
  }
  sdb_mutex_unlock(this->server_lock);

  shutdown(this->master_socket, SHUT_RDWR);
  close(this->master_socket);

  for (auto thread : this->thread_pool) {
    sdb_thread_join_and_destroy(thread);
  }
}

void WebServer::AddServerListener(WebServer::ServerListener *listener) {
  this->listeners.push_back(listener);
}

// TODO: (pburzynski): refactor to send_and_destroy
bool WebServer::SendPacket(int client_id, sdb_packet_t *packet) {
  sdb_mutex_lock(this->server_lock);
  client_info_t *client = this->clients[client_id];

  if (client == nullptr) {
    sdb_mutex_unlock(this->server_lock);
    return false;
  }

  sdb_mutex_lock(client->lock);
  sdb_mutex_unlock(this->server_lock);
  int status = sdb_packet_send(packet, client->socket);
  sdb_mutex_unlock(client->lock);

  return status == 0;
}

int WebServer::AllocateClient(sdb_socket_t socket) {
  sdb_mutex_lock(this->server_lock);

  client_info_t *client = Allocator::New<client_info_t>();
  client->socket = socket;
  client->lock = sdb_mutex_create();

  int client_id = this->next_client_id++;
  this->clients[client_id] = client;

  sdb_mutex_unlock(this->server_lock);
  return client_id;
}

void WebServer::CloseClient(int client_id) {
  sdb_mutex_lock(this->server_lock);
  client_info_t *info = this->clients[client_id];

  if (info == nullptr) {
    sdb_mutex_unlock(this->server_lock);
    return;
  }

  sdb_mutex_lock(info->lock);
  this->clients.erase(client_id);

  sdb_socket_close(info->socket);
  sdb_mutex_destroy(info->lock);

  Allocator::Delete(info);
  sdb_mutex_unlock(this->server_lock);
}

}  // namespace shakadb
