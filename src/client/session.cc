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
#include "session.h"

namespace shakadb {

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
  int request_length = strlen(ping_data);

  this->SendPacket(PacketType::kPing, (uint8_t *)ping_data, request_length);
  data_packet_t *packet = this->ReadPacket();

  if (packet == nullptr) {
    return false;
  }

  bool result = memcmp(ping_data, packet->data, request_length) == 0;
  Allocator::Delete(packet);

  return result;
}

void Session::SendPacket(PacketType type, uint8_t *data, int size) {
  int packet_size = size + sizeof(data_packet_t);
  data_packet_t *packet = (data_packet_t *)Allocator::New<uint8_t *>(packet_size);
  uint8_t *raw_packet = (uint8_t *)packet;

  packet->type = type;
  packet->total_length = packet_size;
  memcpy(packet->data, data, size);

  int sent = 0;
  int total_sent = 0;

  do {
    sent = send(this->sock, raw_packet, packet_size - total_sent, 0);
    raw_packet += sent;
    total_sent += sent;
  } while (sent != 0 && total_sent < size);

  Allocator::Delete(packet);
}

data_packet_t *Session::ReadPacket() {
  data_packet_t header = {0};

  if (recv(this->sock, &header, sizeof(data_packet_t), MSG_WAITALL) != sizeof(data_packet_t)) {
    return nullptr;
  }

  uint8_t *raw_packet = Allocator::New<uint8_t>(header.total_length);
  data_packet_t *packet = (data_packet_t *)raw_packet;

  memcpy(raw_packet, &header, sizeof(data_packet_t));
  raw_packet += sizeof(data_packet_t);

  int total_read = 0;
  int read = 0;
  int data_size = packet->total_length - sizeof(data_packet_t);

  do {
    read = recv(this->sock, raw_packet, data_size - total_read, 0);
    raw_packet += read;
    total_read += read;
  } while (read != 0 && total_read < data_size);

  if (total_read < data_size) {
    Allocator::Delete(packet);
    return nullptr;
  }

  return packet;
}

}

