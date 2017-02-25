//
// Created by Pawel Burzynski on 25/02/2017.
//

#ifndef SHAKADB_WRITERESPONSE_H
#define SHAKADB_WRITERESPONSE_H

#include "data-packet.h"

namespace shakadb {

enum WriteStatus {
  kOk = 1,
  kError = 2
};

class WriteResponse : public DataPacket {
 public:
  WriteResponse();
  WriteResponse(WriteStatus status);

  PacketType GetType();
  WriteStatus GetStatusCount();
 protected:
  bool Deserialize(Buffer *payload);
  std::vector<Buffer *> Serialize();
 private:
  struct write_response_t {
    WriteStatus status;
  };

  WriteStatus status;
};

}

#endif //SHAKADB_WRITERESPONSE_H
