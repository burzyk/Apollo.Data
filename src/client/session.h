//
// Created by Pawel Burzynski on 08/02/2017.
//

#ifndef SHAKADB_STORAGE_SESSION_H
#define SHAKADB_STORAGE_SESSION_H

#include <src/data-point.h>
#include <string>
#include <src/protocol/data-packet.h>
#include <src/protocol/read-response.h>
#include <src/storage/data-point-reader.h>

namespace shakadb {

class Session {
 public:
  static const int kInvalidSocket = -1;

  ~Session();
  static Session *Open(std::string server, int port);

  bool Ping();
  bool WritePoints(std::string series_name, data_point_t *points, int count);
  DataPointReader *ReadPoints(std::string series_name, timestamp_t begin, timestamp_t end);
 private:
  Session(int sock);
  bool SendPacket(DataPacket *packet);
  DataPacket *ReadPacket();
  bool Receive(void *buffer, int size);

  int sock;
};

}

#endif //SHAKADB_STORAGE_SESSION_H
