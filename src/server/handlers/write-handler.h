//
// Created by Pawel Burzynski on 15/02/2017.
//

#ifndef SHAKADB_WRITEHANDLER_H
#define SHAKADB_WRITEHANDLER_H

#include <src/server/server-client.h>
#include <src/server/server.h>
#include <src/middleware/write-queue.h>

namespace shakadb {

class WriteHandler : public Server::ClientConnectedListener, ServerClient::ReceiveListener {
 public:
  WriteHandler(WriteQueue *write_queue);

  void OnClientConnected(Server *server, ServerClient *client);
  void OnReceived(ServerClient *client, data_packet_t *packet);
 private:
  WriteQueue *write_queue;
};

}

#endif //SHAKADB_WRITEHANDLER_H
