//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef APOLLO_STORAGE_UVSERVER_H
#define APOLLO_STORAGE_UVSERVER_H

#include <uv.h>
#include <src/utils/log.h>
#include <list>
#include <vector>
#include <map>
#include "client-handler.h"
#include "server.h"

namespace apollo {

#define A_SERVER_BUFFER_SIZE  65536

class UvServer : public Server {
 public:
  UvServer(int port, int backlog, std::vector<ClientHandler *> handlers, Log *log);
  ~UvServer();

  void Listen();
  void Close();
 private:
  struct client_info_t {

    // TODO: Remove
    int id;

    // TODO: Replace with RingBuffer
    uint8_t *buffer;
    ssize_t buffer_size;
    ssize_t buffer_position;

    // TODO: Remove Debug messages
  };

  static void OnAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
  static void OnDataRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
  static void OnNewConnection(uv_stream_t *server, int status);
  static void OnHandleClose(uv_handle_t *handle);
  static void OnClientShutdown(uv_shutdown_t *req, int status);
  static void OnServerClose(uv_async_t *handle);
  static void OnServerShutdownWatcher(uv_idle_t *handle);

  void RegisterClient(uv_tcp_t *client);
  void RemoveClient(uv_tcp_t *client);
  void HandlePacket(data_packet_t *packet);
  void ReadClientData(client_info_t *info, ssize_t nread, const uv_buf_t *buf);

  int port;
  int backlog;
  int current_client_id;
  bool is_running;

  uv_loop_t event_loop;
  uv_tcp_t server;
  uv_idle_t server_watcher;
  Log *log;
  std::vector<ClientHandler *> handlers;
  std::map<uv_tcp_t *, client_info_t *> clients;
};

}

#endif //APOLLO_STORAGE_UVSERVER_H
