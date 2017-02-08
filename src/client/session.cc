//
// Created by Pawel Burzynski on 08/02/2017.
//

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdlib>
#include "session.h"

namespace apollo {

Session::Session(int sock) {
  this->sock = sock;
}

Session::~Session() {
  if (this->sock != kInvalidSocket) {
    shutdown(this->sock, SHUT_RDWR);
    close(this->sock);
    this->sock = kInvalidSocket;
  }
}

Session *Session::Open(std::string server, int port) {
  struct addrinfo hints = {0};
  struct addrinfo *result;
  int sock = kInvalidSocket;
  std::string port_string = std::to_string(port);

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  if (getaddrinfo(server.c_str(), port_string.c_str(), &hints, &result) != 0) {
    return nullptr;
  }

  for (addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {

    if ((sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == kInvalidSocket) {
      continue;
    }

    if (connect(sock, rp->ai_addr, rp->ai_addrlen) != kInvalidSocket) {
      break;
    }

    close(sock);
    sock = kInvalidSocket;
  }

  freeaddrinfo(result);
  return sock != kInvalidSocket ? new Session(sock) : nullptr;
}

bool Session::Ping() {
  const char *ping_data = "ala ma kota";
  this->SendPacket(PacketType::kPing, (uint8_t *)ping_data, strlen(ping_data));

  return true;
}

void Session::SendPacket(PacketType type, uint8_t *data, int size) {
  int packet_size = size + sizeof(data_packet_t);
  data_packet_t *packet = (data_packet_t *)calloc(packet_size, 1);
  uint8_t *raw_packet = (uint8_t *)packet;

  packet->type = type;
  packet->total_length = packet_size;
  memcpy(packet->data, data, size);

  for (int offset = 0; offset < size; offset += send(this->sock, raw_packet + offset, packet_size - offset, 0));
  free(packet);
}

void Session::ReadResponse(uint8_t *buffer, int size) {

}

}

