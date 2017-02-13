#include <src/utils/log.h>
#include <src/utils/file-log.h>
#include <src/server/server.h>
#include <src/server/uv-server.h>
#include <src/utils/thread.h>
#include <src/server/handlers/packet-logger.h>
#include <src/utils/allocator.h>
#include "fatal-exception.h"

apollo::Log *g_log;
apollo::Server *g_server;
apollo::PacketLogger *g_packet_logger;

void MainRoutine(void *data) {
  g_packet_logger = new apollo::PacketLogger(g_log);
  g_server = new apollo::UvServer(8099, 10, g_log);

  g_server->AddClientConnectedListener(g_packet_logger);

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

  apollo::Allocator::AssertAllDeleted();
}