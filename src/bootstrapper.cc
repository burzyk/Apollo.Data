//
// Created by Pawel Burzynski on 14/02/2017.
//

#include <src/server/web-server.h>
#include <src/file-log.h>
#include <src/utils/allocator.h>
#include <src/storage/standard-database.h>
#include "bootstrapper.h"
#include "configuration.h"

namespace shakadb {

Bootstrapper::Bootstrapper(Configuration *config) {
  this->log = new FileLog(config->GetLogFile());
  this->server = new WebServer(config->GetServerPort(), config->GetServerBacklog(), config->GetServerBacklog(), this->log);
  this->ping_handler = new PingHandler(this->server);
  this->packet_logger = new PacketLogger(this->server, this->log);
  this->db = StandardDatabase::Init(config->GetDbFolder(), this->log, config->GetDbPointsPerChunk(), 0);
  this->write_handler = new WriteHandler(this->db, this->server);
  this->read_handler = new ReadHandler(this->db, this->server, 100);

  this->master_thread = new Thread([this](void *) -> void { this->MainRoutine(); }, this->log);
  this->server_thread = new Thread([this](void *) -> void { this->ServerRoutine(); }, this->log);
}

Bootstrapper::~Bootstrapper() {
  delete this->server_thread;
  delete this->master_thread;

  delete this->read_handler;
  delete this->write_handler;
  delete this->db;
  delete this->packet_logger;
  delete this->ping_handler;
  delete this->server;
  delete this->log;
}

Bootstrapper *Bootstrapper::Run(std::string config_file) {
  Bootstrapper *bootstrapper = new Bootstrapper(Configuration::Load(config_file));
  bootstrapper->master_thread->Start(nullptr);

  return bootstrapper;
}

void Bootstrapper::MainRoutine() {
  this->log->Info("========== Starting ShakaDB ==========");

  this->server_thread->Start(nullptr);

  this->log->Info("ShakaDB started");

  this->control_lock_scope = this->control_lock.Enter();
  this->control_lock_scope->Wait();

  this->log->Info("Stopping the database");

  this->server->Close();
  this->server_thread->Join();

  this->log->Info("========== ShakaDB Stopped ==========");
}

void Bootstrapper::Stop() {
  this->control_lock_scope->Signal();
  this->master_thread->Join();
}

void Bootstrapper::ServerRoutine() {
  // this->server->AddClientConnectedListener(this->packet_logger);
  this->server->AddServerListener(this->ping_handler);
  this->server->AddServerListener(this->write_handler);
  this->server->AddServerListener(this->read_handler);

  this->server->Listen();
}

}