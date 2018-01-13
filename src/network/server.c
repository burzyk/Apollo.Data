//
// Created by Pawel Burzynski on 13/01/2018.
//

#include "src/common.h"
#include "src/network/server.h"

#include "src/utils/memory.h"
#include "src/utils/diagnostics.h"

client_t *client_create(server_t *server, int index);
void client_disconnect_and_destroy(client_t *client);

void on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void on_client_connected(uv_stream_t *master_socket, int status);
void on_data_read(uv_stream_t *client_socket, ssize_t nread, const uv_buf_t *buf);

client_t *client_create(server_t *server, int index) {
  client_t *client = sdb_alloc(sizeof(client_t));
  client->index = index;
  client->buffer_length = 0;
  client->server = server;
  client->buffer_length = 0;
  uv_tcp_init(server->_loop, &client->socket);
  client->socket.data = client;

  return client;
}

void client_disconnect_and_destroy(client_t *client) {
  client->server->_clients[client->index] = NULL;
  uv_close((uv_handle_t *)&client->socket, NULL);
  sdb_free(client);
}

server_t *server_create(int port, packet_handler_t handler, void *handler_context) {
  server_t *server = sdb_alloc(sizeof(server_t));
  server->_port = port;
  server->_loop = uv_default_loop();
  server->_handler = handler;
  server->_handler_context = handler_context;
  uv_tcp_init(server->_loop, &server->_master_socket);
  server->_master_socket.data = server;
  memset(server->_clients, 0, SDB_MAX_CLIENTS * sizeof(client_t *));

  return server;
}

void server_destroy(server_t *server) {
  uv_loop_close(server->_loop);
  sdb_free(server);
}

void server_run(server_t *server) {
  struct sockaddr_in address = {0};
  uv_ip4_addr("0.0.0.0", server->_port, &address);

  if (uv_tcp_bind(&server->_master_socket, (const struct sockaddr *)&address, 0) < 0) {
    die("Failed to bind to the master socket");
  }

  if (uv_listen((uv_stream_t *)&server->_master_socket, 10, on_client_connected) != 0) {
    die("Failed to listen for incoming clients");
  }

  uv_run(server->_loop, UV_RUN_DEFAULT);
  uv_loop_close(server->_loop);
}

void server_stop(server_t *server) {

}

void on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  suggested_size = (size_t)sdb_min(SDB_SERVER_READ_BUFFER_MAX_LEN, (int)suggested_size);

  buf->base = sdb_alloc(suggested_size);
  buf->len = suggested_size;
}

void on_client_connected(uv_stream_t *master_socket, int status) {
  server_t *server = (server_t *)master_socket->data;

  if (status < 0) {
    sdb_log_error("New connection error: %s", uv_strerror(status));
    return;
  }

  sdb_log_debug("Client connected");
  client_t *client = NULL;

  for (int i = 0; i < SDB_MAX_CLIENTS && client == NULL; i++) {
    if (server->_clients[i] == NULL) {
      client = server->_clients[i] = client_create(server, i);
    }
  }

  if (client == NULL) {
    sdb_log_debug("Max clients connected, ignoring this client");
    client_disconnect_and_destroy(client);
    return;
  }

  if (uv_accept((uv_stream_t *)&server->_master_socket, (uv_stream_t *)&client->socket) < 0) {
    sdb_log_error("Failed to accept the connection: %s", uv_strerror(status));
    client_disconnect_and_destroy(client);
  }

  uv_read_start((uv_stream_t *)client, on_alloc, on_data_read);
}

void on_data_read(uv_stream_t *client_socket, ssize_t nread, const uv_buf_t *buf) {
  client_t *client = (client_t *)client_socket->data;

  if (nread < 0) {
    sdb_log_debug("Client disconnected");
    client_disconnect_and_destroy(client);
  } else if (nread > 0) {
    memcpy(client->buffer + client->buffer_length, buf->base, (size_t)nread);
    client->buffer_length += nread;
    sdb_free(buf->base);

    if (client->buffer_length < sizeof(header_t)) {
      return;
    }

    header_t *packet = (header_t *)client->buffer;

    if (packet->magic != SDB_SERVER_MAGIC || packet->total_size > SDB_SERVER_PACKET_MAX_LEN) {
      sdb_log_error("Received malformed packet, disconnecting, magic: %u, len: %u", packet->magic, packet->total_size);
      client_disconnect_and_destroy(client);
      return;
    }

    if (packet->total_size < client->buffer_length) {
      return;
    }

    int result = client->server->_handler(
        client,
        packet->payload,
        packet->total_size - sizeof(header_t),
        client->server->_handler_context);

    if (result) {
      client_disconnect_and_destroy(client);
      return;
    }

    client->buffer_length -= packet->total_size;

    if (client->buffer_length > 0) {
      memcpy(client->buffer, client->buffer + packet->total_size, client->buffer_length);
    }
  }
}
