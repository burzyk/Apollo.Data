//
// Created by Pawel Burzynski on 12/02/2017.
//

#include <src/utils/allocator.h>
#include "uv-server-client.h"
#include "uv-common.h"

namespace apollo {

UvServerClient::UvServerClient(uv_stream_t *client_connection)
    : receive_buffer(65536) {
  this->client_connection = client_connection;

  this->client_connection->data = this;
}

UvServerClient::~UvServerClient() {
  if (this->IsRunning()) {
    throw FatalException("Deleting client that was not closed");
  }
}

UvServerClient *UvServerClient::Accept(uv_stream_t *server, uv_loop_t *loop) {
  uv_tcp_t *client_connection = Allocator::New<uv_tcp_t>();
  uv_tcp_init(loop, client_connection);
  UvServerClient *client = nullptr;

  if (uv_accept(server, (uv_stream_t *)client_connection) == 0) {
    client = new UvServerClient((uv_stream_t *)client_connection);
    uv_read_start((uv_stream_t *)client_connection, UvCommon::OnAlloc, OnDataRead);
  } else {
    uv_close((uv_handle_t *)client_connection, UvCommon::OnHandleClose);
  }

  return client;
}

void UvServerClient::AddReceivedListener(ServerClient::ReceiveListener *listener) {
  this->receive_listeners.push_back(listener);
}

void UvServerClient::AddDisconnectedListener(ServerClient::DisconnectListener *listener) {
  this->disconnect_listeners.push_back(listener);
}

void UvServerClient::SendPacket(PacketType type, uint8_t *data, int data_size) {
  if (!this->IsRunning()) {
    throw FatalException("Sending data to closed client");
  }

  data_packet_t packet = {
      .type = type, .total_length = (int)sizeof(data_packet_t) + data_size
  };
  uv_buf_t buffer[] = {
      {.base = Allocator::New<char>(packet.total_length), .len = (size_t)packet.total_length}
  };
  memcpy(buffer[0].base, &packet, sizeof(data_packet_t));
  memcpy(buffer[0].base + sizeof(data_packet_t), data, data_size);

  uv_write_t *write = Allocator::New<uv_write_t>();
  write->data = buffer[0].base;
  uv_write(write, this->client_connection, buffer, 1, OnDataWrite);
}

void UvServerClient::Close() {
  if (!this->IsRunning()) {
    return;
  }

  uv_read_stop(this->client_connection);

  uv_shutdown_t *shutdown = Allocator::New<uv_shutdown_t>();
  shutdown->data = this;
  uv_shutdown(shutdown, this->client_connection, OnClientShutdown);
}

bool UvServerClient::IsRunning() {
  return this->client_connection != nullptr;
}

void UvServerClient::OnDataRead(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
  UvServerClient *_this = (UvServerClient *)client->data;

  if (nread < 0) {
    _this->Close();
  } else if (nread > 0) {
    _this->ReadData(nread, buf);
  }

  if (buf->base != nullptr && buf->len != 0) {
    Allocator::Delete(buf->base);
  }
}

void UvServerClient::OnDataWrite(uv_write_t *req, int status) {
  Allocator::Delete(req->data);
  Allocator::Delete(req);
}

void UvServerClient::OnClientShutdown(uv_shutdown_t *req, int status) {
  UvServerClient *_this = (UvServerClient *)req->data;

  for (auto listener : _this->disconnect_listeners) {
    listener->OnDisconnected(_this);
  }

  uv_close((uv_handle_t *)_this->client_connection, UvCommon::OnHandleClose);
  _this->client_connection = nullptr;
  Allocator::Delete(req);
}

void UvServerClient::ReadData(ssize_t nread, const uv_buf_t *buf) {
  this->receive_buffer.Write((uint8_t *)buf->base, nread);

  int header_size = sizeof(data_packet_t);
  data_packet_t header = {0};

  while (this->receive_buffer.Peek((uint8_t *)&header, header_size) == header_size
      && (this->receive_buffer.GetSize() >= header.total_length)) {

    uint8_t *packet = Allocator::New<uint8_t>(header.total_length);
    this->receive_buffer.Read(packet, header.total_length);

    for (auto listener: this->receive_listeners) {
      listener->OnReceived(this, (data_packet_t *)packet);
    }

    Allocator::Delete(packet);
  }
}

}

