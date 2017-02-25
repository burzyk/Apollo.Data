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
  bool Deserialize(Buffer *payload);
  std::vector<Buffer *> Serialize();
 private:
  struct read_request_t {
    timestamp_t begin;
    timestamp_t end;
    char series_name[SHAKADB_SERIES_NAME_MAX_LENGTH + 1];
  };

  std::string series_name;
  timestamp_t begin;
  timestamp_t end;
};

}

#endif //SHAKADB_READREQUEST_H
