//
// Created by Pawel Burzynski on 19/02/2017.
//

#include "base-handler.h"

namespace shakadb {

void BaseHandler::OnClientConnected(Server *server, ServerClient *client) {
  client->AddServerClientListener(this);
}

void BaseHandler::OnSend(ServerClient *client, DataPacket *packet) {
  delete packet;
}

void BaseHandler::OnReceived(ServerClient *client, DataPacket *packet) {
}

void BaseHandler::OnDisconnected(ServerClient *client) {
}

}