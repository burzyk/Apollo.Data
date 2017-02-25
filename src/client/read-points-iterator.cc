//
// Created by Pawel Burzynski on 20/02/2017.
//

#include <list>
#include <src/utils/allocator.h>
#include <src/utils/common.h>
#include "read-points-iterator.h"

namespace shakadb {

ReadPointsIterator::ReadPointsIterator(Stream *response) {
  this->response = response;
  this->current = nullptr;
}

ReadPointsIterator::~ReadPointsIterator() {
  if (this->current != nullptr) {
    delete this->current;
  }
}

data_point_t *ReadPointsIterator::CurrentDataPoints() {
  return this->current == nullptr ? nullptr : this->current->GetPoints();
}

int ReadPointsIterator::CurrentDataPointsCount() {
  return this->current == nullptr ? -1 : this->current->GetPointsCount();
}

bool ReadPointsIterator::MoveNext() {
  if (this->current != nullptr) {
    delete this->current;
    this->current = nullptr;
  }

  DataPacket *packet = DataPacket::Load(this->response);

  if (packet == nullptr) {
    return false;
  }

  if (packet->GetType() != kReadResponse) {
    delete packet;
    return false;
  }

  ReadResponse *response = (ReadResponse *)packet;

  if (response->GetPointsCount() == 0) {
    delete response;
    return false;
  }

  this->current = response;
  return true;
}

}