//
// Created by Pawel Burzynski on 13/01/2018.
//

#include "src/network/client-handler.h"

#include <inttypes.h>

#include "src/utils/diagnostics.h"
#include "src/utils/memory.h"
#include "src/utils/network.h"
#include "src/network/protocol.h"

void handle_read(client_t *client, read_request_t *request, sdb_database_t *db);
void handle_write(client_t *client, write_request_t *request, sdb_database_t *db);
void handle_truncate(client_t *client, truncate_request_t *request, sdb_database_t *db);
void handle_read_latest(client_t *client, truncate_request_t *request, sdb_database_t *db);

client_handler_t *client_handler_create(sdb_database_t *db) {
  client_handler_t *handler = (client_handler_t *)sdb_alloc(sizeof(client_handler_t));
  handler->_db = db;

  return handler;
}

void client_handler_destroy(client_handler_t *handler) {
  sdb_free(handler);
}

int client_handler_process_message(client_t *client, uint8_t *data, uint32_t size, void *context) {
  if (packet_validate(data, size)) {
    sdb_log_error("Received packet with malformed payload");
    return 1;
  }

  sdb_database_t *db = ((client_handler_t *)context)->_db;
  packet_header_t *hdr = (packet_header_t *)data;

  sdb_log_debug("packet received, type: %d", hdr->type);
  sdb_stopwatch_t *sw = sdb_stopwatch_start();

  switch (hdr->type) {
    case SDB_READ_REQUEST:handle_read(client, (read_request_t *)data, db);
      break;
    case SDB_WRITE_REQUEST:handle_write(client, (write_request_t *)data, db);
      break;
    case SDB_TRUNCATE_REQUEST:handle_truncate(client, (truncate_request_t *)data, db);
      break;
    case SDB_READ_LATEST_REQUEST:handle_read_latest(client, (read_latest_request_t *)data, db);
      break;
    default:sdb_log_info("Unknown packet type: %d", hdr->type);
      return 1;
  }

  sdb_log_debug("packet handled in: %fs", sdb_stopwatch_stop_and_destroy(sw));
}

void handle_read(client_t *client, read_request_t *request, sdb_database_t *db) {
  sdb_log_debug("processing read request: { series: %d, begin: %"PRIu64", end: %"PRIu64" }",
                request->data_series_id,
                request->begin,
                request->end);

  sdb_timestamp_t begin = request->begin;
  int points_per_packet = sdb_max(1, sdb_min(SDB_POINTS_PER_PACKET_MAX, request->points_per_packet));

  for (;;) {
    int points_to_read = points_per_packet + 1;
    sdb_data_points_reader_t *reader =
        sdb_database_read(db, request->data_series_id, begin, request->end, points_to_read);

    if (reader == NULL) {
      return;
    }

    begin = reader->points_count == points_to_read ? reader->points[reader->points_count - 1].time : request->end;

    int points_to_send = sdb_min(points_per_packet, reader->points_count);
    sdb_log_debug("sending response: { begin: %"PRIu64", end: %"PRIu64", points: %d }",
                  points_to_send ? reader->points[0].time : 0,
                  points_to_send ? reader->points[points_to_send - 1].time : 0,
                  points_to_send);
    int send_status = sdb_packet_send_and_destroy(
        sdb_read_response_create(SDB_RESPONSE_OK, reader->points, points_to_send),
        client);

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

void handle_write(client_t *client, write_request_t *request, sdb_database_t *db) {
  sdb_log_debug("processing write request: { series: %d, points: %d }",
                request->data_series_id,
                request->points_count);

  int status = sdb_database_write(db, request->data_series_id, request->points, request->points_count);

  if (status) {
    sdb_log_error("failed to save data points");
  }

  if (sdb_packet_send_and_destroy(
      sdb_simple_response_create(status ? SDB_RESPONSE_ERROR : SDB_RESPONSE_OK),
      client)) {
    sdb_log_debug("error sending the response");
  }

  sdb_log_debug("all points written");
}

void handle_truncate(client_t *client, truncate_request_t *request, sdb_database_t *db) {
  sdb_log_debug("processing truncate request: { series: %d }", request->data_series_id);

  int status = sdb_database_truncate(db, request->data_series_id);

  if (status) {
    sdb_log_error("failed to truncate data series: %d", request->data_series_id);
  }

  if (sdb_packet_send_and_destroy(
      sdb_simple_response_create(status ? SDB_RESPONSE_ERROR : SDB_RESPONSE_OK),
      client)) {
    sdb_log_debug("error sending the response");
  }

  sdb_log_debug("data series truncated");
}

void handle_read_latest(client_t *client, truncate_request_t *request, sdb_database_t *db) {
  sdb_log_debug("processing read latest request: { series: %d }", request->data_series_id);

  sdb_data_point_t latest = sdb_database_read_latest(db, request->data_series_id);
  sdb_log_debug("latest point: { time: %"PRIu64, ", value: %f }", latest.time, latest.value);

  int send_status = 0;

  if (latest.time != 0) {
    send_status |= sdb_packet_send_and_destroy(sdb_read_response_create(SDB_RESPONSE_OK, &latest, 1), client);
  }

  send_status |= sdb_packet_send_and_destroy(sdb_read_response_create(SDB_RESPONSE_OK, NULL, 0), client);

  if (send_status != 0) {
    sdb_log_debug("error sending response");
    return;
  }
}
