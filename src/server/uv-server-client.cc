//
// Created by Pawel Burzynski on 12/02/2017.
//

#include <src/utils/allocator.h>
#include "uv-server-client.h"
#include "uv-common.h"

namespace shakadb {

// TODO: buffer size initialization
UvServerClient::UvServerClient(uv_stream_t *client_connection, uv_loop_t *loop)
    : receive_buffer(65536) {
  this->client_connection = client_connection;
  this->loop = loop;
  this->is_send_active = false;
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
    client = new UvServerClient((uv_stream_t *)client_connection, loop);
    uv_read_start((uv_stream_t *)client_connection, UvCommon::OnAlloc, OnDataRead);
  } else {
    uv_close((uv_handle_t *)client_connection, UvCommon::OnHandleClose);
  }

  return client;
}

void UvServerClient::AddServerClientListener(ServerClientListener *listener) {
  this->server_client_listeners.push_back(listener);
}

void UvServerClient::SendPacket(DataPacket *packet) {
  if (!this->IsRunning()) {
    throw FatalException("Sending data to closed client");
  }

  auto lock = this->send_queue_monitor.Enter();
  this->send_queue.push_back(packet);
  lock.reset();

  uv_async_t *signal_send = Allocator::New<uv_async_t>();
  uv_async_init(this->loop, signal_send, OnSignalSend);
  signal_send->data = this;
  uv_async_send(signal_send);
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

void UvServerClient::OnSignalSend(uv_async_t *handle) {
  UvServerClient *_this = (UvServerClient *)handle->data;

  if (!_this->is_send_active) {
    _this->SendPendingData();
  }

  uv_close((uv_handle_t *)handle, UvCommon::OnHandleClose);
}

void UvServerClient::OnDataWrite(uv_write_t *req, int status) {
  write_request_t *data = (write_request_t *)req->data;

  if (status < 0) {
    data->client->Close();
  } else {
    data->client->SendPendingData();
  }

  delete data->packet;
  Allocator::Delete(data->buffers);
  Allocator::Delete(data);
  Allocator::Delete(req);
}

void UvServerClient::OnClientShutdown(uv_shutdown_t *req, int status) {
  UvServerClient *_this = (UvServerClient *)req->data;

  for (auto listener : _this->server_client_listeners) {
    listener->OnDisconnected(_this);
  }

  uv_close((uv_handle_t *)_this->client_connection, UvCommon::OnHandleClose);
  _this->client_connection = nullptr;
  Allocator::Delete(req);
}

void UvServerClient::ReadData(ssize_t nread, const uv_buf_t *buf) {
  this->receive_buffer.Write((byte_t *)buf->base, nread);

  DataPacket *packet = nullptr;

  while ((packet = DataPacket::Load(&this->receive_buffer)) != nullptr) {
    for (auto listener: this->server_client_listeners) {
      listener->OnReceived(this, packet);
    }

    delete packet;
  }
}

void UvServerClient::SendPendingData() {
  auto lock = this->send_queue_monitor.Enter();
  DataPacket *packet = nullptr;

  if (this->send_queue.size() > 0) {
    packet = this->send_queue.front();
    this->send_queue.pop_front();
  }
  lock.reset();

  this->is_send_active = packet != nullptr;

  if (!this->is_send_active) {
    return;
  }

  std::vector<Buffer *> fragments = packet->GetFragments();
  write_request_t *data = Allocator::New<write_request_t>();
  data->client = this;
  data->buffers = Allocator::New<uv_buf_t>(fragments.size());
  data->buffers_count = fragments.size();
  data->packet = packet;

  for (int i = 0; i < data->buffers_count; i++) {
    data->buffers[i].base = (char *)fragments[i]->GetBuffer();
    data->buffers[i].len = fragments[i]->GetSize();
  }

  uv_write_t *write = Allocator::New<uv_write_t>();
  write->data = data;
  uv_write(write, this->client_connection, data->buffers, 1, OnDataWrite);
}

}

