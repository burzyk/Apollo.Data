#include <src/utils/log.h>
#include <src/utils/file-log.h>
#include <src/server/server.h>
#include <src/server/uv-server.h>
#include <src/utils/thread.h>
#include "fatal-exception.h"

apollo::Log *g_log;
apollo::Server *g_server;

void MainRoutine(void *data) {
  std::vector<apollo::ClientHandler *> handlers;
  g_server = new apollo::UvServer(8099, 10, handlers, g_log);

  g_server->Listen();
}

int main() {
  g_log = new apollo::FileLog(std::string());
  apollo::Thread main_thread(MainRoutine, g_log);
  main_thread.Start(nullptr);

  g_log->Info("Waiting for termination signal");

  while (getc(stdin) != 'q');

  g_log->Info("Termination signal received - stopping");
  g_server->Close();
  main_thread.Join();

  g_log->Info("Exit");

  delete g_server;
  delete g_log;
}