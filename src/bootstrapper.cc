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
// Created by Pawel Burzynski on 14/02/2017.
//

#include "src/bootstrapper.h"

#include "src/server/web-server.h"
#include "src/file-log.h"
#include "src/utils/allocator.h"
#include "src/storage/standard-database.h"
#include "src/configuration.h"

namespace shakadb {

Bootstrapper::Bootstrapper(Configuration *config) {
  this->log = new FileLog(config->GetLogFile());
  this->server = new WebServer(
      config->GetServerPort(),
      config->GetServerBacklog(),
      config->GetServerBacklog(),
      this->log);
  this->ping_handler = new PingHandler(this->server);
  this->packet_logger = new PacketLogger(this->server, this->log);
  this->db = StandardDatabase::Init(
      config->GetDbFolder(),
      this->log,
      config->GetDbPointsPerChunk(),
      0);
  this->write_handler = new WriteHandler(this->db, this->server);
  this->read_handler = new ReadHandler(this->db, this->server, 65536);

  this->master_thread = new Thread(
      [this](void *) -> void { this->MainRoutine(); },
      this->log);
  this->server_thread = new Thread(
      [this](void *) -> void { this->ServerRoutine(); },
      this->log);
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
  Configuration *config = Configuration::Load(config_file);
  Bootstrapper *bootstrapper = new Bootstrapper(config);
  delete config;

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

}  // namespace shakadb
