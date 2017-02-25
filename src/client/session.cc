//
// Created by Pawel Burzynski on 08/02/2017.
//

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdlib>
#include <src/utils/allocator.h>
#include <src/protocol/ping-packet.h>
#include <src/protocol/write-request.h>
#include <src/protocol/read-request.h>
#include <src/protocol/write-response.h>
#include "session.h"
#include "read-points-iterator.h"

namespace shakadb {

Session::Session(int sock)
    : sock(sock) {
}

Session *Session::Open(std::string server, int port) {
  struct addrinfo hints = {0};
  struct addrinfo *result;
  int sock = -1;
  std::string port_string = std::to_string(port);

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  if (getaddrinfo(server.c_str(), port_string.c_str(), &hints, &result) != 0) {
    return nullptr;
  }

  for (addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {

    if ((sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
      continue;
    }

    if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) {
      break;
    }

    close(sock);
    sock = -1;
  }

  freeaddrinfo(result);
  return sock != -1 ? new Session(sock) : nullptr;
}

bool Session::Ping() {
  const char *ping_data = "ala ma kota";
  int ping_data_length = strlen(ping_data);

  PingPacket request((char *)ping_data, ping_data_length);
  this->SendPacket(&request);

  auto response = std::unique_ptr<PingPacket>((PingPacket *)this->ReadPacket());

  return response != nullptr &&
      request.GetPingDataSize() == response->GetPingDataSize() &&
      memcmp(request.GetPingData(), response->GetPingData(), ping_data_length) == 0;
}

bool Session::WritePoints(std::string series_name, data_point_t *points, int count) {
  WriteRequest request(series_name, points, count);

  if (!this->SendPacket(&request)) {
    return false;
  }

  DataPacket *packet = this->ReadPacket();

  if (packet == nullptr) {
    return false;
  }

  if (packet->GetType() != kWriteResponse) {
    delete packet;
    return false;
  }

  WriteResponse *response = (WriteResponse *)packet;
  bool result = response->GetStatus() == kOk;
  delete response;

  return result;
}

ReadPointsIterator *Session::ReadPoints(std::string series_name, timestamp_t begin, timestamp_t end) {
  ReadRequest request(series_name, begin, end);

  if (!this->SendPacket(&request)) {
    return nullptr;
  }

  return new ReadPointsIterator(&this->sock);
}

bool Session::SendPacket(DataPacket *packet) {
  for (auto fragment: packet->GetFragments()) {
    if (this->sock.Write(fragment->GetBuffer(), fragment->GetSize()) != fragment->GetSize()) {
      return false;
    }
  }

  return true;
}

DataPacket *Session::ReadPacket() {
  return DataPacket::Load(&this->sock);
}

}

