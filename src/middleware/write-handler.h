//
// Created by Pawel Burzynski on 15/02/2017.
//

#ifndef SHAKADB_WRITEHANDLER_H
#define SHAKADB_WRITEHANDLER_H

#include <src/server/server-client.h>
#include <src/server/server.h>
#include <src/database.h>
#include <src/utils/monitor.h>
#include <map>
#include "base-handler.h"

namespace shakadb {

class WriteHandler : public BaseHandler {
 public:
  WriteHandler(Database *db, int buffer_grow_increment, int points_buffer_count);
  ~WriteHandler();

  void OnReceived(ServerClient *client, DataPacket *packet);

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

#endif //SHAKADB_WRITEHANDLER_H
