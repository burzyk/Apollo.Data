//
// Created by Pawel Burzynski on 13/01/2018.
//

#include "src/network/client-handler.h"


void sdb_server_worker_routine(sdb_server_t *server, sdb_socket_t client_socket) {
  sdb_log_debug("client connected: %d", client_socket);
  sdb_packet_t *packet = NULL;

  while ((packet = sdb_packet_receive(client_socket)) != NULL) {
    sdb_log_debug("packet received: { type: %d, length: %d }", packet->header.type, packet->header.payload_size);
    sdb_stopwatch_t *sw = sdb_stopwatch_start();

    switch (packet->header.type) {
      case SDB_READ_REQUEST: sdb_server_handle_read(server, client_socket, packet);
        break;
      case SDB_WRITE_REQUEST:sdb_server_handle_write(server, client_socket, packet);
        break;
      case SDB_TRUNCATE_REQUEST:sdb_server_handle_truncate(server, client_socket, packet);
        break;
      case SDB_READ_LATEST_REQUEST:sdb_server_handle_read_latest(server, client_socket, packet);
        break;
      default: sdb_log_info("Unknown packet type: %d", packet->header.type);
    }

    sdb_log_debug("packet handled in: %fs", sdb_stopwatch_stop_and_destroy(sw));
    sdb_packet_destroy(packet);
  }

  sdb_log_debug("client disconnected: %d", client_socket);
  sdb_socket_close(client_socket);
}

void sdb_server_handle_read(sdb_server_t *server, sdb_socket_t client_socket, sdb_packet_t *packet) {
  sdb_read_request_t *request = (sdb_read_request_t *)packet->payload;
  sdb_log_debug("processing read request: { series: %d, begin: %" PRIu64 ", end: %" PRIu64 "  }",
      request->data_series_id,
      request->begin,
      request->end);

  sdb_timestamp_t begin = request->begin;
  int points_per_packet = sdb_max(1, sdb_min(SDB_POINTS_PER_PACKET_MAX, request->points_per_packet));

  for (;;) {
    int points_to_read = points_per_packet + 1;
    sdb_data_points_reader_t *reader =
        sdb_database_read(server->_db, request->data_series_id, begin, request->end, points_to_read);

    if (reader == NULL) {
      return;
    }

    begin = reader->points_count == points_to_read ? reader->points[reader->points_count - 1].time : request->end;

    int points_to_send = sdb_min(points_per_packet, reader->points_count);
    sdb_log_debug("sending response: { begin: %" PRIu64 ", end: %" PRIu64 ", points: %d }",
        points_to_send ? reader->points[0].time : 0,
        points_to_send ? reader->points[points_to_send - 1].time : 0,
        points_to_send);
    int send_status = sdb_packet_send_and_destroy(
        sdb_read_response_create(SDB_RESPONSE_OK, reader->points, points_to_send),
        client_socket);

    sdb_data_points_reader_destroy(reader);

    if (send_status) {
      sdb_log_debug("error sending the response");
      return;
    }

    if (points_to_send == 0) {
      sdb_log_debug("all points sent");
      return;
    }
  }
}

void sdb_server_handle_write(sdb_server_t *server, sdb_socket_t client_socket, sdb_packet_t *packet) {
  sdb_write_request_t *request = (sdb_write_request_t *)packet->payload;
  sdb_log_debug("processing write request: { series: %d, points: %d }",
                request->data_series_id,
                request->points_count);

  int status = sdb_database_write(server->_db, request->data_series_id, request->points, request->points_count);

  if (status) {
    sdb_log_error("failed to save data points");
  }

  if (sdb_packet_send_and_destroy(
      sdb_simple_response_create(status ? SDB_RESPONSE_ERROR : SDB_RESPONSE_OK),
      client_socket)) {
    sdb_log_debug("error sending the response");
  }

  sdb_log_debug("all points written");
}

void sdb_server_handle_truncate(sdb_server_t *server, sdb_socket_t client_socket, sdb_packet_t *packet) {
  sdb_truncate_request_t *request = (sdb_truncate_request_t *)packet->payload;
  sdb_log_debug("processing truncate request: { series: %d }", request->data_series_id);

  int status = sdb_database_truncate(server->_db, request->data_series_id);

  if (status) {
    sdb_log_error("failed to truncate data series: %d", request->data_series_id);
  }

  if (sdb_packet_send_and_destroy(
      sdb_simple_response_create(status ? SDB_RESPONSE_ERROR : SDB_RESPONSE_OK),
      client_socket)) {
    sdb_log_debug("error sending the response");
  }

  sdb_log_debug("data series truncated");
}

void sdb_server_handle_read_latest(sdb_server_t *server, sdb_socket_t client_socket, sdb_packet_t *packet) {
  int send_status = 0;
  sdb_read_latest_request_t *request = (sdb_read_latest_request_t *)packet->payload;
  sdb_log_debug("processing read latest request: { series: %d }", request->data_series_id);

  sdb_data_point_t latest = sdb_database_read_latest(server->_db, request->data_series_id);
  sdb_log_debug("latest point: { time: %" PRIu64, ", value: %f }", latest.time, latest.value);

  if (latest.time != 0) {
    send_status |= sdb_packet_send_and_destroy(sdb_read_response_create(SDB_RESPONSE_OK, &latest, 1), client_socket);
  }

  send_status |= sdb_packet_send_and_destroy(sdb_read_response_create(SDB_RESPONSE_OK, NULL, 0), client_socket);

  if (send_status != 0) {
    sdb_log_debug("error sending response");
    return;
  }
}
