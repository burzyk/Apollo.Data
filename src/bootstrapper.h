//
// Created by Pawel Burzynski on 14/02/2017.
//

#ifndef SHAKADB_BOOTSTRAPPER_H
#define SHAKADB_BOOTSTRAPPER_H

#include <src/server/server.h>
#include <src/middleware/ping-handler.h>
#include <src/middleware/packet-logger.h>
#include <src/utils/thread.h>
#include <src/middleware/write-handler.h>
#include <src/middleware/read-handler.h>
#include "configuration.h"

namespace shakadb {

class Bootstrapper {
 public:
  virtual ~Bootstrapper();
  static Bootstrapper *Run(std::string config_file);
  void Stop();
 private:
  Bootstrapper(Configuration *config);

  void ServerRoutine();
  void WriteQueueRoutine();
  void MainRoutine();

  Monitor control_lock;
  std::shared_ptr<MonitorScope> control_lock_scope;
  Thread *master_thread;
  Thread *server_thread;
  Thread *write_handler_thread;

  Log *log;
  Server *server;
  PingHandler *ping_handler;
  PacketLogger *packet_logger;
  WriteHandler *write_handler;
  ReadHandler *read_handler;
  Database *db;
};

}

#endif //SHAKADB_BOOTSTRAPPER_H
