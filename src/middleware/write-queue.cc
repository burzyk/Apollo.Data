//
// Created by Pawel Burzynski on 14/02/2017.
//

#include <src/utils/stopwatch.h>
#include <src/utils/allocator.h>
#include "write-queue.h"

namespace shakadb {

// TODO: refactor to a variable
#define SHAKADB_COPY_BUFFER_SIZE  6553600

WriteQueue::WriteQueue(Database *db, int buffer_grow_increment) {
  this->db = db;
  this->is_active = true;
  this->points_buffer = Allocator::New<data_point_t>(SHAKADB_COPY_BUFFER_SIZE);

  // TODO: add initial size to RingBuffer
  this->buffer_grow_increment = buffer_grow_increment * 1000;
  this->points_buffer_size = SHAKADB_COPY_BUFFER_SIZE * sizeof(data_point_t);
}

WriteQueue::~WriteQueue() {
  if (this->is_active) {
    throw FatalException("The queue is still active");
  }

  for (auto buffer: this->buffers) {
    delete buffer.second;
  }

  Allocator::Delete(this->points_buffer);
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

void WriteQueue::Close() {
  auto scope = this->monitor.Enter();
  this->is_active = false;
  scope->Signal();
}

}