//
// Created by Pawel Burzynski on 15/02/2017.
//

#include <src/protocol/data-packet.h>
#include <src/data-point.h>
#include <src/protocol/write-request.h>
#include <src/utils/allocator.h>
#include <src/utils/stopwatch.h>
#include "write-handler.h"

namespace shakadb {

WriteHandler::WriteHandler(Database *db, int buffer_grow_increment, int points_buffer_count) {
  this->db = db;
  this->is_active = true;
  this->points_buffer = Allocator::New<data_point_t>(points_buffer_count);
  this->buffer_grow_increment = buffer_grow_increment;
  this->points_buffer_size = points_buffer_count * sizeof(data_point_t);
}

WriteHandler::~WriteHandler() {
  if (this->is_active) {
    throw FatalException("The queue is still active");
  }

  for (auto buffer: this->buffers) {
    delete buffer.second;
  }

  Allocator::Delete(this->points_buffer);
}

void WriteHandler::OnClientConnected(Server *server, ServerClient *client) {
  client->AddReceivedListener(this);
}

void WriteHandler::OnReceived(ServerClient *client, DataPacket *packet) {
  if (packet->GetType() != kWrite) {
    return;
  }

  WriteRequest *request = (WriteRequest *)packet;

  auto scope = this->monitor.Enter();

  if (this->buffers.find(request->GetSeriesName()) == this->buffers.end()) {
    this->buffers[request->GetSeriesName()] = new RingBuffer(this->buffer_grow_increment);
  }

  this->buffers[request->GetSeriesName()]->Write(
      (uint8_t *)request->GetPoints(),
      request->GetPointsCount() * sizeof(data_point_t));
  scope->Signal();
}

void WriteHandler::ListenForData() {
  auto scope = this->monitor.Enter();

  while (this->is_active) {
    std::map<std::string, RingBuffer *> buffers_snapshot = this->buffers;

    for (auto buffer: buffers_snapshot) {
      Stopwatch sw;
      sw.Start();

      while (buffer.second->GetSize() != 0) {
        int read = buffer.second->Read((uint8_t *)this->points_buffer, this->points_buffer_size) / sizeof(data_point_t);

        scope->Exit();
        this->db->Write(buffer.first, this->points_buffer, read);
        scope->Reenter();
      }

      sw.Stop();
      printf("Elapsed: %fs\n", sw.GetElapsedSeconds());
    }

    scope->Wait();
  }
}

void WriteHandler::Close() {
  auto scope = this->monitor.Enter();
  this->is_active = false;
  scope->Signal();
}

}