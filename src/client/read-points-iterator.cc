/*
 * Copyright (c) 2016 Pawel Burzynski. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
//
// Created by Pawel Burzynski on 20/02/2017.
//

#include "src/client/read-points-iterator.h"

namespace shakadb {

ReadPointsIterator::ReadPointsIterator(sdb_socket_t sock) {
  this->sock = sock;
  this->current = NULL;
}

ReadPointsIterator::~ReadPointsIterator() {
  if (this->current != NULL) {
    sdb_packet_destroy(this->current);
  }
}

data_point_t *ReadPointsIterator::CurrentDataPoints() {
  return this->current == NULL ? NULL : (data_point_t *)((sdb_read_response_t *)this->current->payload)->points;
}

int ReadPointsIterator::CurrentDataPointsCount() {
  return this->current == NULL ? -1 : ((sdb_read_response_t *)this->current->payload)->points_count;
}

bool ReadPointsIterator::MoveNext() {
  if (this->current != NULL) {
    sdb_packet_destroy(this->current);
    this->current = NULL;
  }

  sdb_packet_t *packet = sdb_packet_receive(this->sock);

  if (packet == NULL) {
    return false;
  }

  if (packet->header.type != SDB_READ_RESPONSE) {
    sdb_packet_destroy(packet);
    return false;
  }

  sdb_read_response_t *response = (sdb_read_response_t *)packet->payload;

  if (response->points_count == 0) {
    sdb_packet_destroy(packet);
    return false;
  }

  this->current = packet;
  return true;
}

}  // namespace shakadb
