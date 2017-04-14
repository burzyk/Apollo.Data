//
// Created by Pawel Burzynski on 14/04/2017.
//

#ifndef SHAKADB_NETWORK_H
#define SHAKADB_NETWORK_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int sdb_socket_t;

sdb_socket_t sdb_socket_open();
int sdb_socket_receive(sdb_socket_t socket, void *buffer, size_t size);
int sdb_socket_send(sdb_socket_t socket, void *buffer, size_t size);
void sdb_socket_close(sdb_socket_t socket);


#ifdef __cplusplus
}
#endif

#endif //SHAKADB_NETWORK_H
