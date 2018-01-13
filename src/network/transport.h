//
// Created by Pawel Burzynski on 13/01/2018.
//

#ifndef SHAKADB_TRANSPORT_H
#define SHAKADB_TRANSPORT_H

int packet_send(packet_t *packet, client_t *client);
int packet_send_and_destroy(packet_t *packet, sdb_socket_t socket);
void packet_destroy(packet_t *packet);

#endif //SHAKADB_TRANSPORT_H
