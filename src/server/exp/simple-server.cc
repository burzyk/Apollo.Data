//
// Created by Pawel Burzynski on 25/02/2017.
//

#include <sys/socket.h>
#include <src/fatal-exception.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <src/utils/allocator.h>
#include "simple-server.h"

namespace shakadb {

SimpleServer::SimpleServer(int port, int backlog, int max_clients, Log *log) {
  this->log = log;
  this->port = port;
  this->backlog = backlog;
  this->max_clients = max_clients;
  this->is_running = false;
  this->master_socket = -1;
  this->next_client_id = 10;
}

SimpleServer::~SimpleServer() {
  this->Close();
}

void SimpleServer::Listen() {
  if ((this->master_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    throw FatalException("Unable to open main socket");
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(this->port);
  addr.sin_addr.s_addr = inet_addr("0.0.0.0");

  if (bind(this->master_socket, (struct sockaddr *)&addr, sizeof(sockaddr_in)) == -1) {
    throw FatalException("Unable to bind to the socket");
  }

  if (listen(this->master_socket, this->backlog) == -1) {
    throw FatalException("Unable to listen");
  }

  for (int i = 0; i < this->max_clients; i++) {
    Thread *worker = new Thread([this](void *) -> void { this->WorkerRoutine(); }, this->log);
    this->thread_pool.push_back(worker);
    worker->Start(this);
  }
}

void SimpleServer::WorkerRoutine() {
  while (this->is_running) {
    int client_socket;

    if ((client_socket = accept(this->master_socket, nullptr, nullptr)) != -1) {
      SocketStream *socket = new SocketStream(client_socket);
      int client_id = this->AllocateClient(socket);

      for (auto listener: this->listeners) {
        listener->OnClientConnected(client_id);
      }

      DataPacket *packet = nullptr;

      while ((packet = DataPacket::Load(socket)) != nullptr) {
        for (auto listener: this->listeners) {
          listener->OnPacketReceived(client_id, packet);
        }

        delete packet;
      }

      for (auto listener: this->listeners) {
        listener->OnClientDisconnected(client_id);
      }

      this->CloseClient(client_id);
    }
  }
}

void SimpleServer::Close() {
  auto lock = this->monitor.Enter();
  this->is_running = 0;

  for (auto client: this->clients) {
    client.second->socket->Close();
  }
  lock->Exit();

  shutdown(this->master_socket, SHUT_RDWR);
  close(this->master_socket);

  for (auto thread: this->thread_pool) {
    thread->Join();
    delete thread;
  }
}

void SimpleServer::AddServerListener(SimpleServer::ServerListener *listener) {
  this->listeners.push_back(listener);
}

void SimpleServer::SendPacket(int client_id, DataPacket *packet) {
  auto lock = this->monitor.Enter();
  client_info_t *client = this->clients[client_id];

  if (client == nullptr) {
    return;
  }

  auto client_lock = client->lock->Enter();
  lock->Exit();

  for (auto fragment: packet->GetFragments()) {
    client->socket->Write(fragment->GetBuffer(), fragment->GetSize());
  }
}

int SimpleServer::AllocateClient(SocketStream *socket) {
  auto lock = this->monitor.Enter();
  client_info_t *client = Allocator::New<client_info_t>();
  client->socket = socket;
  client->lock = new Monitor();

  int client_id = this->next_client_id++;
  this->clients[client_id] = client;
  return client_id;
}

void SimpleServer::CloseClient(int client_id) {
  auto lock = this->monitor.Enter();
  client_info_t *info = this->clients[client_id];

  if (info == nullptr) {
    return;
  }

  auto info_lock = info->lock->Enter();
  this->clients.erase(client_id);
  delete info->socket;
  delete info->lock;
  Allocator::Delete(info);
}

}