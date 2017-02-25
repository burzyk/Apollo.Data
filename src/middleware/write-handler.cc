//
// Created by Pawel Burzynski on 15/02/2017.
//

#include <src/protocol/data-packet.h>
#include <src/data-point.h>
#include <src/protocol/write-request.h>
#include <src/utils/allocator.h>
#include <src/utils/stopwatch.h>
#include <src/utils/memory-buffer.h>
#include <src/protocol/write-response.h>
#include "write-handler.h"

namespace shakadb {

WriteHandler::WriteHandler(Database *db, Server *server)
    : BaseHandler(server) {
  this->db = db;
}
void WriteHandler::OnPacketReceived(int client_id, DataPacket *packet) {
  if (packet->GetType() != kWriteRequest) {
    return;
  }

  WriteRequest *request = (WriteRequest *)packet;
  this->db->Write(request->GetSeriesName(), request->GetPoints(), request->GetPointsCount());

  WriteResponse response(kOk);
  this->GetServer()->SendPacket(client_id, &response);
}

}