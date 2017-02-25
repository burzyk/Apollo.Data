//
// Created by Pawel Burzynski on 19/02/2017.
//

#include "base-handler.h"

namespace shakadb {

BaseHandler::BaseHandler(Server *server) {
  this->server = server;
}

void BaseHandler::OnClientConnected(int client_id) {
}

void BaseHandler::OnClientDisconnected(int client_id) {
}

void BaseHandler::OnPacketReceived(int client_id, DataPacket *packet) {
}

Server *BaseHandler::GetServer() {
  return this->server;
}

}