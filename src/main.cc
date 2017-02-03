#include <src/utils/log.h>
#include <src/utils/file-log.h>
#include <src/server/server.h>
#include <src/server/uv-server.h>
#include "fatal-exception.h"

apollo::Server *g_server;

void exit_handler(int signal) {
  if (signal != SIGINT) {
    return;
  }

  if (g_server != nullptr) {
    g_server->Close();
  }
}

void init_signals() {
  struct sigaction sa = {0};
  sa.sa_handler = exit_handler;
  sa.sa_flags = SA_RESTART;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);
}

int main() {
  apollo::Log *log = new apollo::FileLog(std::string());

  try {
    std::vector<apollo::ClientHandler *> handlers;
    g_server = new apollo::UvServer(8099, 10, handlers, log);

    init_signals();
    g_server->Listen();

    delete g_server;
    delete log;

    return 0;
  } catch (apollo::FatalException ex) {
    log->Fatal(std::string(ex.what()));
    return -1;
  }
}