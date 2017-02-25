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
  WriteHandler(Database *db);
  ~WriteHandler();

  void OnReceived(ServerClient *client, DataPacket *packet);

  void ListenForData();
  void Close();
 private:
  struct write_info_t {
    std::string series_name;
    Buffer *points;
    int points_count;
  };

  Database *db;
  std::list<write_info_t> write_info;
  volatile bool is_active;
  Monitor monitor;
};

}

#endif //SHAKADB_WRITEHANDLER_H
