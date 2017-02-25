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
#include "session.h"
#include "client-data-points-reader.h"

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
  return this->SendPacket(&request);
}

DataPointsReader *Session::ReadPoints(std::string series_name, timestamp_t begin, timestamp_t end) {
  ReadRequest request(series_name, begin, end);
  this->SendPacket(&request);
  return new ClientDataPointsReader([this]() -> ReadResponse * { return (ReadResponse *)this->ReadPacket(); });
}

bool Session::SendPacket(DataPacket *packet) {
//  byte_t *raw_packet = packet->GetPacket();
//  int packet_size = packet->GetPacketSize();
//  int sent = 0;
//  int total_sent = 0;
//
//  do {
//    sent = send(this->sock, raw_packet, packet_size - total_sent, 0);
//    raw_packet += sent;
//    total_sent += sent;
//  } while (sent != 0 && total_sent < packet_size);
//
//  return total_sent == packet_size;
}

DataPacket *Session::ReadPacket() {
//  data_packet_header_t header;
//
//  if (!this->Receive((byte_t *)&header, sizeof(header))) {
//    return nullptr;
//  }
//
//  byte_t *raw_packet = Allocator::New<byte_t>(header.packet_length);
//  memcpy(raw_packet, &header, sizeof(header));
//
//  if (!this->Receive(raw_packet + sizeof(data_packet_header_t), header.packet_length - sizeof(data_packet_header_t))) {
//    Allocator::Delete(raw_packet);
//    return nullptr;
//  }
//
//  return PacketLoader::Load(raw_packet, header.packet_length);
}

bool Session::Receive(byte_t *buffer, int size) {
  ssize_t read = 0;

  while ((read = recv(this->sock, buffer, size, MSG_WAITALL)) > 0) {
    size -= read;
    buffer += read;
  }

  return size == 0;
}

}

