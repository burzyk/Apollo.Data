//
// Created by Pawel Burzynski on 14/02/2017.
//

#ifndef SHAKADB_WRITEQUEUE_H
#define SHAKADB_WRITEQUEUE_H

#include <string>
#include <src/data-point.h>
#include <src/storage/database.h>
#include <src/utils/ring-buffer.h>

namespace shakadb {

class WriteQueue {
 public:
  WriteQueue(Database *db);
  ~WriteQueue();

  void Enqueue(std::string series_name, data_point_t *points, int points_count);
  void ListenForData();
  void Close();
 private:
  std::map<std::string, RingBuffer *> buffers;
  Database *db;
  volatile bool is_active;
};

}

#endif //SHAKADB_WRITEQUEUE_H
