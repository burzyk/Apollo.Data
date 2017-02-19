//
// Created by Pawel Burzynski on 12/02/2017.
//

#ifndef SHAKADB_STORAGE_UVSERVERCLIENT_H
#define SHAKADB_STORAGE_UVSERVERCLIENT_H

#include <uv.h>
#include <list>
#include <src/utils/ring-buffer.h>
#include "src/protocol/data-packet.h"
#include "server-client.h"

namespace shakadb {

class UvServerClient : public ServerClient {
 public:
  ~UvServerClient();
  static UvServerClient *Accept(uv_stream_t *server, uv_loop_t *loop);

  void AddServerClientListener(ServerClientListener *listener);
  void SendPacket(DataPacket *packet);
  void Close();
  bool IsRunning();
 private:
  UvServerClient(uv_stream_t *client_connection);

  static void OnDataRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
  static void OnDataWrite(uv_write_t *req, int status);
  static void OnClientShutdown(uv_shutdown_t *req, int status);

  void ReadData(ssize_t nread, const uv_buf_t *buf);

  uv_stream_t *client_connection;
  RingBuffer receive_buffer;
  std::list<ServerClientListener *> server_client_listeners;
};

}

#endif //SHAKADB_STORAGE_UVSERVERCLIENT_H
