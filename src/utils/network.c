//
// Created by Pawel Burzynski on 14/04/2017.
//

#include "src/utils/network.h"

#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <src/common.h>
#include <netinet/in.h>
#include <arpa/inet.h>

sdb_socket_t sdb_socket_listen(int port, int backlog) {
  signal(SIGPIPE, SIG_IGN);
  sdb_socket_t sock;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    return -1;
  }

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr("0.0.0.0");

  if (bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    return -1;
  }

  if (listen(sock, backlog) == -1) {
    return -1;
  }

  return sock;
}

int sdb_socket_receive(sdb_socket_t socket, void *buffer, size_t size) {
  ssize_t read = 0;
  int total_read = 0;
  char *ptr = (char *)buffer;

  while ((read = recv(socket, ptr, size, 0)) > 0) {
    ptr += read;
    size -= read;
    total_read += read;
  }

  return total_read;
}

int sdb_socket_send(sdb_socket_t socket, void *buffer, size_t size) {
  ssize_t sent = 0;
  int total_send = 0;
  char *ptr = (char *)buffer;

  while ((sent = send(socket, ptr, size, 0)) > 0) {
    ptr += sent;
    size -= sent;
    total_send += sent;
  }

  return total_send;
}

void sdb_socket_close(sdb_socket_t socket) {
  shutdown(socket, SHUT_RDWR);
  close(socket);
}

sdb_socket_t sdb_socket_accept(sdb_socket_t socket) {
  return accept(socket, NULL, NULL);
}
