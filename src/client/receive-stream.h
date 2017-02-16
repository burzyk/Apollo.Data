//
// Created by Pawel Burzynski on 17/02/2017.
//

#ifndef SHAKADB_RECEIVESTREAM_H
#define SHAKADB_RECEIVESTREAM_H

#include <src/utils/stream.h>
#include <src/utils/ring-buffer.h>

namespace shakadb {

#define SHAKA_CURRENT_BUFFER_SIZE 65536

class ReceiveStream : public Stream {
 public:
  ReceiveStream(int socket, int received_buffer_grow);
  ~ReceiveStream();

  int Read(uint8_t *buffer, int buffer_size);
  int Peek(uint8_t *buffer, int buffer_size);
  void Write(uint8_t *buffer, int buffer_size);
  bool HasData(int size);
 private:
  void LoadMissing(int requested_size);

  int socket;
  RingBuffer *total_recv_buffer;
  uint8_t *current_recv_buffer;
};

}

#endif //SHAKADB_RECEIVESTREAM_H
