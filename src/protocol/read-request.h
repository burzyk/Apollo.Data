//
// Created by Pawel Burzynski on 19/02/2017.
//

#ifndef SHAKADB_READREQUEST_H
#define SHAKADB_READREQUEST_H

#include <string>
#include <src/data-point.h>
#include "data-packet.h"

namespace shakadb {

class ReadRequest : public DataPacket {
 public:
  ReadRequest(byte_t *raw_packet, int packet_size);
  ReadRequest(std::string series_name, timestamp_t begin, timestamp_t end);

  PacketType GetType();
  std::string GetSeriesName();
  timestamp_t GetBegin();
  timestamp_t GetEnd();
 private:
  int GetSeriesNameSize();
};

}

#endif //SHAKADB_READREQUEST_H
