//
// Created by Pawel Burzynski on 12/02/2017.
//

#ifndef APOLLO_STORAGE_UVSERVERCLIENT_H
#define APOLLO_STORAGE_UVSERVERCLIENT_H

#include <uv.h>
#include <list>
#include <src/utils/ring-buffer.h>
#include "data-packet.h"
#include "server-client.h"

namespace apollo {

class UvServerClient : public ServerClient {
 public:
  ~UvServerClient();
  static UvServerClient *Accept(uv_stream_t *server, uv_loop_t *loop);

  void AddReceivedListener(ReceiveListener *listener);
  void AddDisconnectedListener(DisconnectListener *listener);
  void SendPacket(PacketType type, uint8_t *data, int data_size);
  void Close();
  bool  IsRunning();
 private:
  UvServerClient(uv_stream_t *client_connection);

  static void OnDataRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
  static void OnDataWrite(uv_write_t* req, int status);
  static void OnClientShutdown(uv_shutdown_t *req, int status);

  void ReadData(ssize_t nread, const uv_buf_t *buf);

  uv_stream_t *client_connection;
  RingBuffer receive_buffer;
  std::list<ReceiveListener *> receive_listeners;
  std::list<DisconnectListener *> disconnect_listeners;
};

}

#endif //APOLLO_STORAGE_UVSERVERCLIENT_H
