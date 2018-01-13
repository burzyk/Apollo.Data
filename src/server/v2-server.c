//
// Created by Pawel Burzynski on 13/01/2018.
//

#include "src/server/v2-server.h"

#include "src/utils/memory.h"
#include "src/utils/diagnostics.h"

client_t *client_create(uv_loop_t *loop, int index);
void client_disconnect_and_destroy(client_t *client);

void on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void on_client_connected(uv_stream_t *master_socket, int status);
void on_data_read(uv_stream_t *client_socket, ssize_t nread, const uv_buf_t *buf);

client_t *client_create(uv_loop_t *loop, int index) {
  client_t *client = sdb_alloc(sizeof(client_t));
  client->index = index;
  client->buffer_length = 0;
  uv_tcp_init(loop, &client->socket);
  client->socket.data = client;

  return client;
}

void client_disconnect_and_destroy(client_t *client) {
  client->server->_clients[client->index] = NULL;
  uv_close((uv_handle_t *)&client->socket, NULL);
  sdb_free(client);
}

server_t *server_create(int port, sdb_database_t *db) {
  server_t *server = sdb_alloc(sizeof(server_t));
  server->_port = port;
  server->_db = db;
  server->_loop = uv_default_loop();
  server->_stop = 0;
  uv_tcp_init(server->_loop, &server->_master_socket);
  server->_master_socket.data = server;
  memset(server->_clients, 0, SDB_MAX_CLIENTS * sizeof(client_t *));

  return server;
}

void server_destroy(server_t *server) {

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
      client = server->_clients[i] = client_create(server->_loop, i);
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
    if (nread + client->buffer_length > SDB_PACKET_MAX_LEN) {
      sdb_log_info("Client exceeded read buffer, disconnecting");
      client_disconnect_and_destroy(client);
    }

    memcpy(client->buffer + client->buffer_length, buf->base, buf->len);
    // TODO: call read callback
  }

  if (buf->base != NULL && buf->len != 0) {
    sdb_free(buf->base);
  }
}