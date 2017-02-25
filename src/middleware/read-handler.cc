//
// Created by Pawel Burzynski on 19/02/2017.
//

#include <src/protocol/read-request.h>
#include <src/protocol/read-response.h>
#include <src/utils/common.h>
#include <src/utils/allocator.h>
#include "read-handler.h"

namespace shakadb {

ReadHandler::ReadHandler(Database *db, Server *server, int points_per_packet)
    : BaseHandler(server) {
  this->db = db;
  this->points_per_packet = points_per_packet;
}

void ReadHandler::OnPacketReceived(int client_id, DataPacket *packet) {
  if (packet->GetType() != kReadRequest) {
    return;
  }

  ReadRequest *request = (ReadRequest *)packet;
  timestamp_t begin = request->GetBegin();
  bool send_more_data = true;

  while (send_more_data) {
    auto reader = std::unique_ptr<DataPointsReader>(
        this->db->Read(request->GetSeriesName(), begin, request->GetEnd(), this->points_per_packet));
    ReadResponse response(reader.get(), begin == request->GetBegin() ? 0 : 1);

    if (!this->GetServer()->SendPacket(client_id, &response)) {
      return;
    }

    begin = reader->GetDataPoints()[reader->GetDataPointsCount() - 1].time;
    send_more_data = reader->GetDataPointsCount() != 0;
  }
}

}