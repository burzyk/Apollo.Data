//
// Created by Pawel Burzynski on 14/02/2017.
//

#ifndef SHAKADB_WRITEQUEUE_H
#define SHAKADB_WRITEQUEUE_H

#include <string>
#include <src/data-point.h>
#include <src/storage/database.h>
#include <src/utils/ring-buffer.h>
#include <src/utils/monitor.h>
#include <map>

namespace shakadb {

class WriteQueue {
 public:
  WriteQueue(Database *db, int buffer_grow_increment);
  ~WriteQueue();

  void Enqueue(std::string series_name, data_point_t *points, int points_count);
  void ListenForData();
  void Close();
 private:
  Database *db;
  data_point_t *points_buffer;
  int points_buffer_size;
  std::map<std::string, RingBuffer *> buffers;
  volatile bool is_active;
  int buffer_grow_increment;
  Monitor monitor;
};

}

#endif //SHAKADB_WRITEQUEUE_H
