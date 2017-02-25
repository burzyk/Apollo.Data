//
// Created by Pawel Burzynski on 25/02/2017.
//

#ifndef SHAKADB_SOCKETSTREAM_H
#define SHAKADB_SOCKETSTREAM_H

#include "stream.h"

namespace shakadb {

class SocketStream : public Stream {
 public:
  SocketStream(int socket);
  ~SocketStream();

  int Read(byte_t *buffer, int buffer_size);
  void Write(byte_t *buffer, int buffer_size);
  void Close();
 private:
  int socket;
};

}

#endif //SHAKADB_SOCKETSTREAM_H
