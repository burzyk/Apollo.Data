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
// Created by Pawel Burzynski on 19/02/2017.
//

#include "client-handler.h"

namespace shakadb {

ClientHandler::ClientHandler(Server *server, Database *db, int points_per_packet) {
  this->server = server;
  this->db = db;
  this->points_per_packet = points_per_packet;
}

void ClientHandler::OnPacketReceived(int client_id, sdb_packet_t *packet) {
  if (packet->header.type == SDB_READ_REQUEST) {

    sdb_read_request_t *request = (sdb_read_request_t *)packet->payload;
    timestamp_t begin = request->begin;

    while (true) {
      auto reader = std::unique_ptr<DataPointsReader>(
          this->db->Read(request->data_series_id, begin, request->end, this->points_per_packet));

      int skip = begin == request->begin || reader.get()->GetDataPointsCount() == 0 ? 0 : 1;
      int sent_points_count = reader->GetDataPointsCount() - skip;
      sdb_data_point_t *points = (sdb_data_point_t *)&reader->GetDataPoints()[skip];

      sdb_packet_t *response = sdb_read_response_create(SDB_RESPONSE_OK, points, sent_points_count);
      int send_status = this->server->SendPacket(client_id, response);
      sdb_packet_destroy(response);

      if (!send_status) {
        return;
      }

      if (sent_points_count == 0) {
        break;
      }

      begin = reader->GetDataPoints()[reader->GetDataPointsCount() - 1].time;
    }
  }

  if (packet->header.type == SDB_WRITE_REQUEST) {

    sdb_write_request_t *request = (sdb_write_request_t *)packet->payload;
    int status = this->db->Write(request->data_series_id, request->points, request->points_count);

    sdb_packet_t *response = sdb_write_response_create(status ? SDB_RESPONSE_ERROR : SDB_RESPONSE_OK);
    this->server->SendPacket(client_id, response);
    sdb_packet_destroy(response);
  }
}

}  // namespace shakadb
