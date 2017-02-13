//
// Created by Pawel Burzynski on 14/02/2017.
//

#ifndef SHAKADB_BOOTSTRAPPER_H
#define SHAKADB_BOOTSTRAPPER_H

#include <src/server/server.h>
#include <src/server/handlers/ping-handler.h>
#include <src/server/handlers/packet-logger.h>
#include <src/utils/thread.h>

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

  Thread *server_thread;

  Log *log;
  Server *server;
  PingHandler *ping_handler;
  PacketLogger *packet_logger;
};

}

#endif //SHAKADB_BOOTSTRAPPER_H
