//
// Created by Pawel Burzynski on 14/02/2017.
//

#include <src/server/uv-server.h>
#include <src/file-log.h>
#include <src/utils/allocator.h>
#include "bootstrapper.h"

namespace shakadb {

Bootstrapper::Bootstrapper() {
  this->log = new FileLog(std::string());
  this->server = new UvServer(8099, 10, this->log);
  this->ping_handler = new PingHandler();
  this->packet_logger = new PacketLogger(this->log);
  this->server_thread = new Thread([this](void *) -> void { this->ServerRoutine(); }, this->log);
}

Bootstrapper::~Bootstrapper() {
  delete this->server_thread;
  delete this->packet_logger;
  delete this->ping_handler;
  delete this->server;
  delete this->log;
}

void Bootstrapper::Run() {
  Bootstrapper *bootstrapper = new Bootstrapper();

  bootstrapper->Start();

  while (getc(stdin) != 'q');

  bootstrapper->Stop();

  delete bootstrapper;
  Allocator::AssertAllDeleted();
}

void Bootstrapper::Start() {
  this->log->Info("========== Starting ShakaDB ==========");

  this->server_thread->Start(nullptr);

  this->log->Info("ShakaDB started");
}

void Bootstrapper::Stop() {
  this->log->Info("Stopping the database");

  this->server->Close();
  this->server_thread->Join();

  this->log->Info("========== ShakaDB Stopped ==========");
}

void Bootstrapper::ServerRoutine() {
  this->server->AddClientConnectedListener(this->packet_logger);
  this->server->AddClientConnectedListener(this->ping_handler);

  this->server->Listen();
}

}