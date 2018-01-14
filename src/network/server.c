//
// Created by Pawel Burzynski on 13/01/2018.
//

#include "src/network/server.h"

#include "src/common.h"
#include "src/diagnostics.h"
#include "src/network/protocol.h"

client_t *client_create(server_t *server, int index);
void client_disconnect_and_destroy(client_t *client);

void on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void on_client_connected(uv_stream_t *master_socket, int status);
void on_data_read(uv_stream_t *client_socket, ssize_t nread, const uv_buf_t *buf);
void on_write_complete(uv_write_t *req, int status);

client_t *client_create(server_t *server, int index) {
  client_t *client = sdb_alloc(sizeof(client_t));
  client->index = index;
  client->buffer_length = 0;
  client->server = server;
  client->buffer_length = 0;
  uv_tcp_init(server->loop, &client->socket);
  client->socket.data = client;

  return client;
}

void client_disconnect_and_destroy(client_t *client) {
  client->server->clients[client->index] = NULL;
  uv_close((uv_handle_t *)&client->socket, NULL);
  sdb_free(client);
}

int client_send_and_destroy_data(client_t *client, uint8_t *data, size_t count) {
  uv_write_t *request = (uv_write_t *)sdb_alloc(sizeof(uv_write_t));
  uv_buf_t *buf = (uv_buf_t *)sdb_alloc(sizeof(uv_buf_t));
  request->data = buf;
  buf->base = (char *)data;
  buf->len = count;

  int status;

  if ((status = uv_write(request, (uv_stream_t *)&client->socket, buf, 1, on_write_complete)) >= 0) {
    return 0;
  }

  log_error("Failed to write to client: %s", uv_strerror(status));
  on_write_complete(request, -1);
  return status;
}

void on_write_complete(uv_write_t *req, int status) {
  uv_buf_t *buf = (uv_buf_t *)req->data;

  sdb_free(buf->base);
  sdb_free(buf);
  sdb_free(req);
}

server_t *server_create(int port, packet_handler_t handler, void *handler_context) {
  server_t *server = sdb_alloc(sizeof(server_t));
  server->port = port;
  server->loop = uv_default_loop();
  server->handler = handler;
  server->handler_context = handler_context;
  uv_tcp_init(server->loop, &server->master_socket);
  server->master_socket.data = server;
  memset(server->clients, 0, SDB_MAX_CLIENTS * sizeof(client_t *));

  return server;
}

void server_destroy(server_t *server) {
  uv_loop_close(server->loop);
  sdb_free(server);
}

void server_run(server_t *server) {
  struct sockaddr_in address = {0};
  uv_ip4_addr("0.0.0.0", server->port, &address);

  if (uv_tcp_bind(&server->master_socket, (const struct sockaddr *)&address, 0) < 0) {
    die("Failed to bind to the master socket");
  }

  if (uv_listen((uv_stream_t *)&server->master_socket, 10, on_client_connected) != 0) {
    die("Failed to listen for incoming clients");
  }

  uv_run(server->loop, UV_RUN_DEFAULT);
  uv_loop_close(server->loop);
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
    log_error("New connection error: %s", uv_strerror(status));
    return;
  }

  log_debug("Client connected");
  client_t *client = NULL;

  for (int i = 0; i < SDB_MAX_CLIENTS && client == NULL; i++) {
    if (server->clients[i] == NULL) {
      client = server->clients[i] = client_create(server, i);
    }
  }

  if (client == NULL) {
    log_debug("Max clients connected, ignoring this client");
    return;
  }

  if ((status = uv_accept((uv_stream_t *)&server->master_socket, (uv_stream_t *)&client->socket)) < 0) {
    log_error("Failed to accept the connection: %s", uv_strerror(status));
    client_disconnect_and_destroy(client);
  }

  if ((status = uv_read_start((uv_stream_t *)client, on_alloc, on_data_read)) < 0) {
    log_error("Failed to start reading: %s", uv_strerror(status));
    client_disconnect_and_destroy(client);
  }
}

void on_data_read(uv_stream_t *client_socket, ssize_t nread, const uv_buf_t *buf) {
  client_t *client = (client_t *)client_socket->data;

  if (nread < 0) {
    log_debug("Client disconnected, %d", client->index);
    client_disconnect_and_destroy(client);
  } else if (nread > 0) {
    memcpy(client->buffer + client->buffer_length, buf->base, (size_t)nread);
    client->buffer_length += nread;
    sdb_free(buf->base);

    if (client->buffer_length < sizeof(packet_t)) {
      return;
    }

    packet_t *packet = (packet_t *)client->buffer;

    if (packet->magic != SDB_SERVER_MAGIC || packet->total_size > SDB_SERVER_PACKET_MAX_LEN) {
      log_error("Received malformed packet, disconnecting, magic: %u, len: %u", packet->magic, packet->total_size);
      client_disconnect_and_destroy(client);
      return;
    }

    if (packet->total_size < client->buffer_length) {
      return;
    }

    int result = client->server->handler(
        client,
        packet->payload,
        packet->total_size - sizeof(packet_t),
        client->server->handler_context);

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
