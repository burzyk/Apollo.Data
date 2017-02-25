//
// Created by Pawel Burzynski on 20/02/2017.
//

#include <list>
#include <src/utils/allocator.h>
#include <src/utils/common.h>
#include "client-data-points-reader.h"

namespace shakadb {

ClientDataPointsReader::ClientDataPointsReader(std::function<ReadResponse *()> packet_provider) {
//  this->points_count = 0;
//  this->position = 0;
//  this->received_points = nullptr;
//
//  std::list<ReadResponse *> packets;
//
//  do {
//    ReadResponse *response = packet_provider();
//    this->points_count += response->GetPointsCount();
//    packets.push_back(response);
//  } while (this->points_count < packets.front()->GetTotalPointsCount());
//
//  this->received_points = Allocator::New<data_point_t>(this->points_count);
//  int curr = 0;
//
//  for (auto packet: packets) {
//    memcpy(this->received_points + curr, packet->GetPoints(), packet->GetPointsCount() * sizeof(data_point_t));
//    curr += packet->GetPointsCount();
//  }
}

ClientDataPointsReader::~ClientDataPointsReader() {
  if (this->received_points != nullptr) {
    Allocator::Delete(this->received_points);
  }
}

int ClientDataPointsReader::GetDataPointsCount() {
  return this->points_count;
}

data_point_t *ClientDataPointsReader::GetDataPoints() {
  return nullptr;
}

}