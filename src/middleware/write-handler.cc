//
// Created by Pawel Burzynski on 15/02/2017.
//

#include <src/protocol/data-packet.h>
#include <src/data-point.h>
#include <src/protocol/write-request.h>
#include <src/utils/allocator.h>
#include <src/utils/stopwatch.h>
#include <src/utils/memory-buffer.h>
#include "write-handler.h"

namespace shakadb {

WriteHandler::WriteHandler(Database *db) {
  this->db = db;
  this->is_active = true;
}

WriteHandler::~WriteHandler() {
  if (this->is_active) {
    throw FatalException("The queue is still active");
  }

  for (auto buffer: this->write_info) {
    delete buffer.points;
  }
}

void WriteHandler::OnReceived(ServerClient *client, DataPacket *packet) {
  if (packet->GetType() != kWriteRequest) {
    return;
  }

  WriteRequest *request = (WriteRequest *)packet;
  write_info_t info = {
      .points=new MemoryBuffer(request->GetPointsCount() * sizeof(data_point_t)),
      .series_name=request->GetSeriesName(),
      .points_count=request->GetPointsCount()};

  memcpy(info.points->GetBuffer(), request->GetPoints(), info.points->GetSize());

  auto scope = this->monitor.Enter();
  this->write_info.push_back(info);
  scope->Signal();
}

void WriteHandler::ListenForData() {
  auto scope = this->monitor.Enter();

  while (this->is_active) {
    std::list<write_info_t> write_info_snapshot = this->write_info;
    scope->Exit();

    for (auto info: write_info_snapshot) {
      data_point_t *points = (data_point_t *)info.points->GetBuffer();
      std::sort(points, points + info.points_count, [](data_point_t p1, data_point_t p2) {
        return p1.time < p2.time;
      });

      this->db->Write(info.series_name, points, info.points_count);
      delete info.points;
    }

    scope->Reenter();
    scope->Wait();
  }
}

void WriteHandler::Close() {
  auto scope = this->monitor.Enter();
  this->is_active = false;
  scope->Signal();
}

}