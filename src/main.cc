#include <src/utils/log.h>
#include <src/utils/file-log.h>
#include <src/server/server.h>
#include <src/server/uv-server.h>
#include "fatal-exception.h"

apollo::Server *g_server;

int main() {
  apollo::Log *log = new apollo::FileLog(std::string());

  try {
    std::vector<apollo::ClientHandler *> handlers;
    g_server = new apollo::UvServer(8099, 10, handlers, log);

    // TODO: run in different thread
    g_server->Listen();

    delete g_server;
    delete log;

    return 0;
  } catch (apollo::FatalException ex) {
    log->Fatal(std::string(ex.what()));
    return -1;
  }
}