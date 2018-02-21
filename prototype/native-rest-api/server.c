//
// Created by Pawel Burzynski on 13/01/2018.
//

#include "server.h"

#include <string.h>
#include <stdlib.h>

void die(char *msg) {
  fprintf(stderr, msg);
  exit(-1);
}

client_t *client_create(server_t *server, int index);
void client_disconnect_and_destroy(client_t *client);

void on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
void on_client_connected(uv_stream_t *master_socket, int status);
void on_data_read(uv_stream_t *client_socket, ssize_t nread, const uv_buf_t *buf);
void on_write_complete(uv_write_t *req, int status);
void on_server_shutdown(uv_async_t *request);
void on_client_free(uv_handle_t *socket);

client_t *client_create(server_t *server, int index) {
  client_t *client = calloc(sizeof(client_t), 1);
  client->index = index;
  client->buffer_length = 0;
  client->server = server;
  client->buffer_length = 0;
  uv_tcp_init(&server->loop, &client->socket);
  client->socket.data = client;

  // if this is not available on linux
  // each time a connection is reset by a peer
  // the application will crash
  signal(SIGPIPE, SIG_IGN);

  return client;
}

void client_disconnect_and_destroy(client_t *client) {
  client->server->clients[client->index] = NULL;
  uv_close((uv_handle_t *)&client->socket, on_client_free);
}

void on_client_free(uv_handle_t *socket) {
  free(socket->data);
}

int client_send_and_destroy_data(client_t *client, uint8_t *data, size_t count) {
  uv_write_t *request = (uv_write_t *)calloc(sizeof(uv_write_t), 1);
  uv_buf_t *buf = (uv_buf_t *)calloc(sizeof(uv_buf_t), 2);
  request->data = buf;

  buf->base = (char *)data;
  buf->len = count;
  buf[1].base = client;

  int status;

  if ((status = uv_write(request, (uv_stream_t *)&client->socket, buf, 1, on_write_complete)) >= 0) {
    return 0;
  }

  on_write_complete(request, -1);
  return status;
}

void on_write_complete(uv_write_t *req, int status) {
  uv_buf_t *buf = (uv_buf_t *)req->data;

  client_disconnect_and_destroy(buf[1].base);

  free(buf->base);
  free(buf);
  free(req);


}

server_t *server_create(int port, packet_handler_t handler, void *handler_context) {
  server_t *server = calloc(sizeof(server_t), 1);
  server->port = port;
  uv_loop_init(&server->loop);
  server->handler = handler;
  server->handler_context = handler_context;
  uv_tcp_init(&server->loop, &server->master_socket);
  server->shutdown_request.data = NULL;
  uv_async_init(&server->loop, &server->shutdown_request, on_server_shutdown);
  server->master_socket.data = server;
  memset(server->clients, 0, SDB_MAX_CLIENTS * sizeof(client_t *));

  return server;
}

void server_destroy(server_t *server) {
  uv_loop_close(&server->loop);
  free(server);
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

  if (uv_run(&server->loop, UV_RUN_DEFAULT) != 0) {
    die("Failed to start the main loop");
  }
}

void server_stop(server_t *server) {
  server->shutdown_request.data = server;
  uv_async_send(&server->shutdown_request);
}

void on_server_shutdown(uv_async_t *request) {
  server_t *server = (server_t *)request->data;

  if (server == NULL) {
    return;
  }

  uv_close((uv_handle_t *)&server->master_socket, NULL);

  for (int i = 0; i < SDB_MAX_CLIENTS; i++) {
    client_t *client = server->clients[i];

    if (client != NULL) {
      client_disconnect_and_destroy(client);
    }
  }

  uv_close((uv_handle_t *)request, NULL);
}

void on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  suggested_size = SDB_SERVER_READ_BUFFER_MAX_LEN;

  buf->base = calloc(suggested_size, 1);
  buf->len = suggested_size;
}

void on_client_connected(uv_stream_t *master_socket, int status) {
  server_t *server = (server_t *)master_socket->data;

  if (status < 0) {
    return;
  }

  int new_client_index = 0;

  while (new_client_index < SDB_MAX_CLIENTS && server->clients[new_client_index] != NULL) {
    new_client_index++;
  }

  if (new_client_index >= SDB_MAX_CLIENTS) {
    return;
  }

  client_t *client = server->clients[new_client_index] = client_create(server, new_client_index);

  if ((status = uv_accept((uv_stream_t *)&server->master_socket, (uv_stream_t *)&client->socket)) < 0) {
    client_disconnect_and_destroy(client);
  }

  if ((status = uv_read_start((uv_stream_t *)client, on_alloc, on_data_read)) < 0) {
    client_disconnect_and_destroy(client);
  }
}

void on_data_read(uv_stream_t *client_socket, ssize_t nread, const uv_buf_t *buf) {
  client_t *client = (client_t *)client_socket->data;

  if (nread < 0) {
    const char *data =
        "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"ala\": \"ma kota\"}\r\n";
    size_t data_len = strlen(data);

    char *response = calloc(data_len, 1);
    strncpy(response, data, data_len);

    client_send_and_destroy_data(client, response, data_len);

  } else if (nread > 0) {
    memcpy(client->buffer + client->buffer_length, buf->base, (size_t)nread);
    client->buffer_length += nread;
    free(buf->base);
  }
}
