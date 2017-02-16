//
// Created by Pawel Burzynski on 15/02/2017.
//

#include <src/server/data-packet.h>
#include <src/data-point.h>
#include "write-handler.h"

namespace shakadb {

WriteHandler::WriteHandler(WriteQueue *write_queue) {
  this->write_queue = write_queue;
}

void WriteHandler::OnClientConnected(Server *server, ServerClient *client) {
  client->AddReceivedListener(this);
}

void WriteHandler::OnReceived(ServerClient *client, data_packet_t *packet) {
  if (packet->type != kWrite) {
    return;
  }

  write_packet_t *write_packet = (write_packet_t *)packet->data;
  std::string series_name = std::string(write_packet->series_name);
  data_point_t *points = (data_point_t *)(packet->data + series_name.size() + 1);
  int points_count = (packet->payload_length() - series_name.size() - 1) / sizeof(data_point_t);

  // TODO: Integrate WriteQueue with WriteHandler
  this->write_queue->Enqueue(series_name, points, points_count);
}

}