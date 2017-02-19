//
// Created by Pawel Burzynski on 20/02/2017.
//

#include <list>
#include <src/utils/allocator.h>
#include "client-data-point-reader.h"

namespace shakadb {

ClientDataPointReader::ClientDataPointReader(std::function<ReadResponse *()> packet_provider) {
  this->points_count = 0;
  this->points = nullptr;

  std::list<ReadResponse *> packets;

  do {
    ReadResponse *response = packet_provider();
    this->points_count += response->GetPointsCount();
    packets.push_back(response);
  } while (this->points_count < packets.front()->GetTotalPointsCount());

  this->points = Allocator::New<data_point_t>(this->points_count);
  int position = 0;

  for (auto packet: packets) {
    memcpy(this->points + position, packet->GetPoints(), packet->GetPointsCount());
    position += packet->GetPointsCount();
  }
}

ClientDataPointReader::~ClientDataPointReader() {
  if (this->points != nullptr) {
    Allocator::Delete(this->points);
  }
}

data_point_t *ClientDataPointReader::GetDataPoints() {
  return this->points;
}

int ClientDataPointReader::GetDataPointsCount() {
  return this->points_count;
}

}