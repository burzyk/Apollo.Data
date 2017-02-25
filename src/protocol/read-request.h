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
  ReadRequest();
  ReadRequest(std::string series_name, timestamp_t begin, timestamp_t end);

  PacketType GetType();
  std::string GetSeriesName();
  timestamp_t GetBegin();
  timestamp_t GetEnd();
 protected:
  void Deserialize(Buffer *payload);
  std::vector<Buffer *> Serialize();
 private:
  std::string series_name;
  timestamp_t begin;
  timestamp_t end;
};

}

#endif //SHAKADB_READREQUEST_H
