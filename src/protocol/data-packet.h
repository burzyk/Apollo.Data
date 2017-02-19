//
// Created by Pawel Burzynski on 01/02/2017.
//

#ifndef SHAKADB_STORAGE_DATA_PACKET_H
#define SHAKADB_STORAGE_DATA_PACKET_H

#include <cstdint>
#include <src/utils/ring-buffer.h>
#include <memory>

namespace shakadb {

enum PacketType {
  kPing = 1,
  kWriteRequest = 2,
  kWriteResponse = 3,
  kReadRequest = 4,
  kReadResponse = 5
};

struct data_packet_header_t {
  PacketType type;
  int packet_length;
};

class DataPacket {
 public:
  DataPacket();
  DataPacket(uint8_t *raw_packet, int packet_size);
  virtual ~DataPacket();

  virtual PacketType GetType() = 0;
  uint8_t *GetPacket();
  int GetPacketSize();
 protected:
  void InitPacket(int payload_size);
  uint8_t *GetPayload();
  int GetPayloadSize();
 private:
  uint8_t *raw_packet;
  int packet_size;
};

}

#endif //SHAKADB_STORAGE_DATA_PACKET_H
