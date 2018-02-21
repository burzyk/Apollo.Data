#include <stdio.h>
#include "server.h"

int main() {
  server_t *server = server_create(8090, NULL, NULL);
  server_run(server);
}