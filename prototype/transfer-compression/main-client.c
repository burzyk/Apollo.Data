
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include "lz4.h"

typedef struct stopwatch_s {
  uint64_t start;
  uint64_t stop;
} stopwatch_t;

uint64_t sdb_now() {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  return (uint64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
}

stopwatch_t *stopwatch_start() {
  stopwatch_t *stopwatch = (stopwatch_t *)calloc(sizeof(stopwatch_t), 1);
  stopwatch->start = sdb_now();

  return stopwatch;
}

float stopwatch_stop_and_destroy(stopwatch_t *stopwatch) {
  stopwatch->stop = sdb_now();

  float elapsed = (stopwatch->stop - stopwatch->start) / 1000000000.0f;

  free(stopwatch);
  return elapsed;
}

int socket_connect(const char *server, int port) {
  struct addrinfo hints = {0};
  struct addrinfo *result;
  int sock = -1;
  char port_string[1024] = {0};
  snprintf(port_string, 1024, "%d", port);

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(server, port_string, &hints, &result) != 0) {
    return sock;
  }

  for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
    if ((sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
      continue;
    }
    if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) {
      break;
    }

    close(sock);
    sock = -1;
  }

  freeaddrinfo(result);
  return sock;
}

int main() {
  while (1) {
    stopwatch_t *sw = stopwatch_start();
    int count = 1000;
    int size = 655360;

    char *data = malloc(size);
    char *compressed = malloc(size);

    for (int i = 0; i < size / 4; i++) {
      ((int *)data)[i] = i;
    }

    int sock = socket_connect("localhost", 9087);

    if (sock == -1) {
      return -1;
    }

    int compressed_size = 0;

    for (int i = 0; i < count; i++) {
      compressed_size = LZ4_compress(data, compressed, size);
//      compressed_size = size;
//      memcpy(compressed, data, size);

      send(sock, &size, sizeof(size), 0);
      send(sock, &compressed_size, sizeof(compressed_size), 0);

      int sent = 0;
      while ((sent += send(sock, compressed + sent, compressed_size - sent, 0)) > 0 && sent != compressed_size);
    }

    close(sock);
    free(data);
    free(compressed);

    printf("Processed: %d (%d) in %f\n", count * size, count * compressed_size, stopwatch_stop_and_destroy(sw));
  }
}