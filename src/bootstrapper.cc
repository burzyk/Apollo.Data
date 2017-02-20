//
// Created by Pawel Burzynski on 14/02/2017.
//

#include <src/server/uv-server.h>
#include <src/file-log.h>
#include <src/utils/allocator.h>
#include <src/storage/standard-database.h>
#include "bootstrapper.h"
#include "configuration.h"

namespace shakadb {

Bootstrapper::Bootstrapper(Configuration *config) {
  this->log = new FileLog(config->GetLogFile());
  this->server = new UvServer(config->GetServerPort(), config->GetServerBacklog(), this->log);
  this->ping_handler = new PingHandler();
  this->packet_logger = new PacketLogger(this->log);
  this->db = StandardDatabase::Init(config->GetDbFolder(), this->log, config->GetDbPointsPerChunk(), 0);
  this->write_handler = new WriteHandler(
      this->db,
      config->GetWriteHandlerBufferSize(),
      config->GetWriteHandlerBufferSize());
  this->read_handler = new ReadHandler(this->db, config->GetReadHandlerBufferSize());

  this->write_handler_thread = new Thread([this](void *) -> void { this->WriteQueueRoutine(); }, this->log);
  this->server_thread = new Thread([this](void *) -> void { this->ServerRoutine(); }, this->log);
}

Bootstrapper::~Bootstrapper() {
  delete this->server_thread;
  delete this->write_handler_thread;

  delete this->read_handler;
  delete this->write_handler;
  delete this->db;
  delete this->packet_logger;
  delete this->ping_handler;
  delete this->server;
  delete this->log;
}

void Bootstrapper::Run(std::string config_file) {
  Bootstrapper *bootstrapper = new Bootstrapper(Configuration::Load(config_file));
  bootstrapper->log->Info("========== Starting ShakaDB ==========");

  bootstrapper->Start();

  while (getc(stdin) != 'q');

  bootstrapper->Stop();

  bootstrapper->log->Info("========== ShakaDB Stopped ==========");
  delete bootstrapper;
  Allocator::AssertAllDeleted();
}

void Bootstrapper::Start() {
  this->server_thread->Start(nullptr);
  this->write_handler_thread->Start(nullptr);

  this->log->Info("ShakaDB started");
}

void Bootstrapper::Stop() {
  this->log->Info("Stopping the database");

  this->server->Close();
  this->server_thread->Join();

  this->write_handler->Close();
  this->write_handler_thread->Join();
}

void Bootstrapper::ServerRoutine() {
  // this->server->AddClientConnectedListener(this->packet_logger);
  this->server->AddServerListener(this->ping_handler);
  this->server->AddServerListener(this->write_handler);
  this->server->AddServerListener(this->read_handler);

  this->server->Listen();
}

void Bootstrapper::WriteQueueRoutine() {
  this->write_handler->ListenForData();
}

}