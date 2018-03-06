#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "lz4.h"
#include <math.h>
#include <time.h>

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

int test_lz4() {
  FILE *f = fopen("/Users/pburzynski/data/test.data", "rb");
  uint64_t file_size = 1000000000;
  char *data = malloc(file_size);
  char *compressed = malloc(file_size);
  char *decompressed = malloc(file_size);

  uint64_t read = fread(data, 1, file_size, f);

  for (int i = 0; i < 100; i++) {
    stopwatch_t *sw = stopwatch_start();

    int compressed_size = LZ4_compress(data, compressed, file_size);
    LZ4_decompress_safe(compressed, decompressed, compressed_size, file_size);

    float ratio = compressed_size / (float)file_size;

    if (memcmp(decompressed, data, file_size)) {
      return -1;
    }

    printf("Processed, %f, %fs\n", ratio, stopwatch_stop_and_destroy(sw));
  }

  free(decompressed);
  free(compressed);
  free(data);
}

void generate_random_file() {
  int size = 1000000;
  int *data = (int *)malloc(sizeof(int) * size);
  int *deltas = (int *)malloc(sizeof(int) * size);
  srand((unsigned int)time(NULL));

  for (int i = 0; i < size; i++) {
    data[i] = i + (rand() * 100) % 50;
  }

  for (int i = 0; i < size - 1; i++) {
    deltas[i] = data[i - 1] - data[i];
  }

  FILE *f = fopen("/Users/pburzynski/data/test.data", "wb");
  fwrite(data, sizeof(int), size, f);

  FILE *d = fopen("/Users/pburzynski/data/test.data-delta", "wb");
  fwrite(deltas, sizeof(int), size, d);
}

int main() {
  generate_random_file();

//  test_lz4();
  return 0;

  int sock = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in addr = {0};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9087);
  addr.sin_addr.s_addr = inet_addr("0.0.0.0");

  bind(sock, (const struct sockaddr *)&addr, sizeof(addr));
  listen(sock, 10);

  while (1) {
    int client = accept(sock, NULL, NULL);

    if (client == -1) {
      return -1;
    }

    while (1) {
      int size = 0;
      int compress_size = 0;
      int read = 0;
      recv(client, &size, sizeof(size), 0);
      recv(client, &compress_size, sizeof(size), 0);

      char *data = malloc(size);
      char *compress_data = malloc(compress_size);

      while ((read += recv(client, compress_data + read, compress_size - read, 0)) > 0 && read != compress_size);

      if (read != compress_size || read == 0) {
        break;
      }

      LZ4_decompress_safe(compress_data, data, compress_size, size);
//      memcpy(data, compress_data, size);

      for (int i = 0; i < size / 4; i++) {
        if (((int *)data)[i] != i) {
          printf("ERROR");
        }
      }

      free(data);
      free(compress_data);
    }
  }
}