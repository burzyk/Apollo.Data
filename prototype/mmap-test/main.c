#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>

#define FILE_NAME "/Users/pburzynski/projects/ShakaDB/prototype/mmap-test/test.file"

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

void mmap_test() {
  FILE *f = fopen(FILE_NAME, "rb+");
  fseek(f, 0, SEEK_END);
  off_t file_size = ftello(f);

  char *map = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fileno(f), 0);
  int acc = 0;
  stopwatch_t *sw = stopwatch_start();

  for (int i = 0; i < file_size; i++) {
    acc += map[i];
    //acc = acc % 17;
  }

  printf("Total read in: %f, result: %d (mmap)\n", stopwatch_stop_and_destroy(sw), acc);
  munmap(map, file_size);
  fclose(f);
}

void fread_test() {
  FILE *f = fopen(FILE_NAME, "rb+");
  fseek(f, 0, SEEK_SET);
  char *buff = calloc(655360, 1);
  int acc = 0;
  stopwatch_t *sw = stopwatch_start();

  while (!feof(f)) {
    size_t read_count = fread(buff, 1, 655360, f);

    for (size_t i = 0; i < read_count; i++) {
      acc += buff[i];
    }
  }

  printf("Total read in: %f, result: %d (fread)\n", stopwatch_stop_and_destroy(sw), acc);
  free(buff);
  fclose(f);
}

void no_read_test() {
  FILE *f = fopen(FILE_NAME, "rb+");
  fseek(f, 0, SEEK_END);
  off_t file_size = ftello(f);

  int acc = 0;
  stopwatch_t *sw = stopwatch_start();

  for (size_t i = 0; i < file_size; i++) {
    acc += 123;
  }

  printf("Total read in: %f, result: %d (no_read)\n", stopwatch_stop_and_destroy(sw), acc);
  fclose(f);
}

int main() {
  while (1) {
    mmap_test();
    fread_test();
    no_read_test();
  }
}