//
// Created by Pawel Burzynski on 14/02/2017.
//

#ifndef SHAKADB_BOOTSTRAPPER_H
#define SHAKADB_BOOTSTRAPPER_H

#include <src/server/server.h>
#include <src/server/handlers/ping-handler.h>
#include <src/server/handlers/packet-logger.h>
#include <src/utils/thread.h>
#include <src/server/handlers/write-handler.h>

namespace shakadb {

class Bootstrapper {
 public:
  static void Run();
 private:
  Bootstrapper();
  ~Bootstrapper();

  void Start();
  void Stop();
  void ServerRoutine();
  void WriteQueueRoutine();

  Thread *server_thread;
  Thread *write_queue_thread;

  Log *log;
  Server *server;
  PingHandler *ping_handler;
  PacketLogger *packet_logger;
  WriteHandler *write_handler;
  WriteQueue *write_queue;
  Database *db;
};

}

#endif //SHAKADB_BOOTSTRAPPER_H
