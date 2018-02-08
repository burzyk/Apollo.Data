
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

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

void call_service() {
  int sock = socket_connect("localhost", 8090);
  const char *data =
      "HTTP/1.1 200 OK\r\n"
          "Content-Type: application/json\r\n"
          "\r\n"
          "{\"ala\": \"ma kota\"}\r\n";
  size_t data_len = strlen(data);

  send(sock, data, data_len, 0);
  close(sock);
}

int main() {
  while (1) {
    stopwatch_t *sw = stopwatch_start();
    int count = 10000;

    for (int i = 0; i < count; i++) {
      call_service();
    }

    printf("Processed: %d in %f\n", count, stopwatch_stop_and_destroy(sw));
  }
}