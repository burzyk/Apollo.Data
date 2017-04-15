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

#include "src/client/data-points-iterator.h"
#include "src/utils/memory.h"

sdb_data_points_iterator_t *sdb_data_points_iterator_create(sdb_socket_t sock) {
  sdb_data_points_iterator_t *iterator = (sdb_data_points_iterator_t *)sdb_alloc(sizeof(sdb_data_points_iterator_t));
  iterator->_sock = sock;
  iterator->_current = NULL;
  iterator->points = NULL;
  iterator->points_count = -1;

  return iterator;
}

void sdb_data_points_iterator_destroy(sdb_data_points_iterator_t *iterator) {
  if (iterator->_current != NULL) {
    sdb_packet_destroy(iterator->_current);
  }

  sdb_free(iterator);
}

int sdb_data_points_iterator_next(sdb_data_points_iterator_t *iterator) {
  if (iterator->_current != NULL) {
    sdb_packet_destroy(iterator->_current);
    iterator->_current = NULL;
    iterator->points = NULL;
    iterator->points_count = -1;
  }

  sdb_packet_t *packet = sdb_packet_receive(iterator->_sock);

  if (packet == NULL) {
    return 0;
  }

  if (packet->header.type != SDB_READ_RESPONSE) {
    sdb_packet_destroy(packet);
    return 0;
  }

  sdb_read_response_t *response = (sdb_read_response_t *)packet->payload;

  if (response->points_count == 0) {
    sdb_packet_destroy(packet);
    return 0;
  }

  iterator->_current = packet;
  iterator->points = response->points;
  iterator->points_count = response->points_count;

  return 1;
}


