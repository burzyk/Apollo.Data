//
// Created by Pawel Burzynski on 14/04/2017.
//

#ifndef SRC_UTILS_NETWORK_H_
#define SRC_UTILS_NETWORK_H_

#include <stddef.h>

typedef int sdb_socket_t;

#define SDB_INVALID_SOCKET  ((sdb_socket_t)-1)

sdb_socket_t sdb_socket_listen(int port, int backlog);
sdb_socket_t sdb_socket_connect(const char *hostname, int port);
int sdb_socket_receive(sdb_socket_t socket, void *buffer, size_t size);
int sdb_socket_send(sdb_socket_t socket, void *buffer, size_t size);
void sdb_socket_close(sdb_socket_t socket);
sdb_socket_t sdb_socket_accept(sdb_socket_t socket);

#endif  // SRC_UTILS_NETWORK_H_
