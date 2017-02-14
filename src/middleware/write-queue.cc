//
// Created by Pawel Burzynski on 14/02/2017.
//

#include "write-queue.h"

namespace shakadb {

#define SHAKADB_COPY_BUFFER_SIZE  65536

WriteQueue::WriteQueue(Database *db, int buffer_grow_increment) {
  this->db = db;
  this->is_active = true;
  this->buffer_grow_increment = buffer_grow_increment;
}

WriteQueue::~WriteQueue() {
  if (this->is_active) {
    this->Close();
  }

  for (auto buffer: this->buffers) {
    delete buffer.second;
  }
}

void WriteQueue::Enqueue(std::string series_name, data_point_t *points, int points_count) {
  auto scope = this->monitor.Enter();

  if (this->buffers.find(series_name) == this->buffers.end()) {
    this->buffers[series_name] = new RingBuffer(this->buffer_grow_increment);
  }

  this->buffers[series_name]->Write((uint8_t *)points, points_count * sizeof(data_point_t));
  scope->Signal();
}

void WriteQueue::ListenForData() {
  data_point_t points[SHAKADB_COPY_BUFFER_SIZE] = {0};
  auto scope = this->monitor.Enter();

  while (this->is_active) {
    std::map<std::string, RingBuffer *> buffers_snapshot = this->buffers;

    for (auto buffer: buffers_snapshot) {
      while (buffer.second->GetSize() != 0) {
        int read = buffer.second->Read((uint8_t *)points, SHAKADB_COPY_BUFFER_SIZE) / sizeof(data_point_t);

        scope->Exit();
        this->db->Write(buffer.first, points, read);
        scope->Reenter();
      }
    }

    scope->Wait();
  }
}

void WriteQueue::Close() {
  auto scope = this->monitor.Enter();
  this->is_active = false;
  scope->Signal();
}

}