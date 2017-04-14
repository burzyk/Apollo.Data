//
// Created by Pawel Burzynski on 14/04/2017.
//

#include "src/utils/network.h"

#include <sys/socket.h>
#include <unistd.h>

sdb_socket_t sdb_socket_open() {
  return socket(AF_INET, SOCK_STREAM, 0);
}

int sdb_socket_receive(sdb_socket_t socket, void *buffer, size_t size) {
  ssize_t read = 0;
  int total_read = 0;

  while ((read = recv(socket, buffer, size, 0)) > 0) {
    buffer += read;
    size -= read;
    total_read += read;
  }

  return total_read;
}

int sdb_socket_send(sdb_socket_t socket, void *buffer, size_t size) {
  ssize_t sent = 0;
  int total_send = 0;

  while ((sent = send(socket, buffer, size, 0)) > 0) {
    buffer += sent;
    buffer -= sent;
    total_send += sent;
  }

  return total_send;
}

void sdb_socket_close(sdb_socket_t socket) {
  shutdown(socket, SHUT_RDWR);
  close(socket);
}
