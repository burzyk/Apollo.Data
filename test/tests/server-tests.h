//
// Created by Pawel Burzynski on 03/02/2017.
//

#ifndef SHAKADB_STORAGE_SERVER_TESTS_H
#define SHAKADB_STORAGE_SERVER_TESTS_H

#include <test/framework/test-context.h>
#include <thread>
#include <src/server/server.h>
#include <src/server/uv-server.h>
#include <test/common.h>
#include <src/middleware/packet-logger.h>

namespace shakadb {
namespace test {

void server_thread_routine(Server *server) {
  server->Listen();
}

void server_initialize_test(TestContext ctx) {
  Log *log = new NullLog();
  std::vector handlers;
  handlers.push_back(new PacketLogger(log));
  Server *srv = new UvServer(8099, 10, handlers, log);

  std::thread server_thread(server_thread_routine, srv);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  srv->Close();
  delete srv;
}

}
}

#endif //SHAKADB_STORAGE_SERVER_TESTS_H
