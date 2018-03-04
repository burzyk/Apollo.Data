//
// Created by Pawel Burzynski on 13/01/2018.
//

#ifndef SRC_NETWORK_CLIENT_HANDLER_H_
#define SRC_NETWORK_CLIENT_HANDLER_H_

#include "src/storage/database.h"
#include "src/network/server.h"

#ifndef SDB_READ_MAX_PAYLOAD
// 300 is just an arbitrary number to
// reduce any risk of hitting the transport limit
#define SDB_READ_MAX_PAYLOAD (SDB_SERVER_PACKET_MAX_LEN - 300)
#endif

typedef struct client_handler_s {
  database_t *db;
} client_handler_t;

client_handler_t *client_handler_create(database_t *db);
void client_handler_destroy(client_handler_t *handler);
int client_handler_process_message(client_t *client, uint8_t *data, size_t size, client_handler_t *handler);

#endif  // SRC_NETWORK_CLIENT_HANDLER_H_
