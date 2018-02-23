//
// Created by Pawel Burzynski on 13/01/2018.
//

#include "src/network/client-handler.h"

#include <inttypes.h>

#include "src/diagnostics.h"
#include "src/network/protocol.h"

void handle_read(client_t *client, read_request_t *request, database_t *db);
void handle_write(client_t *client, write_request_t *request, database_t *db);
void handle_truncate(client_t *client, truncate_request_t *request, database_t *db);
void handle_read_latest(client_t *client, read_latest_request_t *request, database_t *db);
int send_and_destroy(client_t *client, buffer_t packet);

client_handler_t *client_handler_create(database_t *db) {
  client_handler_t *handler = (client_handler_t *)sdb_alloc(sizeof(client_handler_t));
  handler->db = db;

  return handler;
}

void client_handler_destroy(client_handler_t *handler) {
  sdb_free(handler);
}

int client_handler_process_message(client_t *client, uint8_t *data, size_t size, client_handler_t *handler) {
  if (!payload_validate(data, size)) {
    log_error("Received packet with malformed payload");
    return 1;
  }

  database_t *db = handler->db;
  payload_header_t *hdr = (payload_header_t *)data;

  log_debug("packet received, type: %d", hdr->type);
  stopwatch_t *sw = stopwatch_start();

  switch (hdr->type) {
    case SDB_READ_REQUEST:handle_read(client, (read_request_t *)data, db);
      break;
    case SDB_WRITE_REQUEST:handle_write(client, (write_request_t *)data, db);
      break;
    case SDB_TRUNCATE_REQUEST:handle_truncate(client, (truncate_request_t *)data, db);
      break;
    case SDB_READ_LATEST_REQUEST:handle_read_latest(client, (read_latest_request_t *)data, db);
      break;
    default:log_info("Unknown packet type: %d", hdr->type);
      return 2;
  }

  log_debug("packet handled in: %fs", stopwatch_stop_and_destroy(sw));
  return 0;
}

void handle_read(client_t *client, read_request_t *request, database_t *db) {
  log_debug("processing read request: { series: %d, begin: %"PRIu64", end: %"PRIu64" }",
            request->data_series_id,
            request->begin,
            request->end);

  timestamp_t begin = request->begin;
  uint64_t points_per_packet = sdb_max(1, sdb_min(SDB_POINTS_PER_PACKET_MAX, request->points_per_packet));

  for (;;) {
    uint64_t points_to_read = points_per_packet + 1;
    points_reader_t *reader =
        database_read(db, request->data_series_id, begin, request->end, points_to_read);

    if (reader == NULL) {
      return;
    }

    begin = reader->points_count == points_to_read ? reader->points[reader->points_count - 1].time : request->end;

    uint64_t points_to_send = sdb_min(points_per_packet, reader->points_count);
    log_debug("sending response: { begin: %"PRIu64", end: %"PRIu64", points: %d }",
              points_to_send ? reader->points[0].time : 0,
              points_to_send ? reader->points[points_to_send - 1].time : 0,
              points_to_send);
    int send_status = send_and_destroy(client, read_response_create(reader->points, points_to_send));

    points_reader_destroy(reader);

    if (send_status) {
      log_debug("error sending the response");
      return;
    }

    if (points_to_send == 0) {
      log_debug("all points sent");
      return;
    }
  }
}

void handle_write(client_t *client, write_request_t *request, database_t *db) {
  log_debug("processing write request: { series: %d, points: %d }",
            request->data_series_id,
            request->points_count);

  int status = database_write(db, request->data_series_id, request->points, request->points_count);

  if (status) {
    log_error("failed to save data points");
  }

  if (send_and_destroy(client, simple_response_create(status ? SDB_RESPONSE_ERROR : SDB_RESPONSE_OK))) {
    log_debug("error sending the response");
  }

  log_debug("all points written");
}

void handle_truncate(client_t *client, truncate_request_t *request, database_t *db) {
  log_debug("processing truncate request: { series: %d }", request->data_series_id);

  int status = database_truncate(db, request->data_series_id);

  if (status) {
    log_error("failed to truncate data series: %d", request->data_series_id);
  }

  if (send_and_destroy(client, simple_response_create(status ? SDB_RESPONSE_ERROR : SDB_RESPONSE_OK))) {
    log_debug("error sending the response");
  }

  log_debug("data series truncated");
}

void handle_read_latest(client_t *client, read_latest_request_t *request, database_t *db) {
  log_debug("processing read latest request: { series: %d }", request->data_series_id);

  data_point_t latest = database_read_latest(db, request->data_series_id);
  log_debug("latest point: { time: %"PRIu64, ", value: %f }", latest.time, latest.value);

  data_point_t *result = latest.time != 0 ? &latest : NULL;
  uint64_t count = result != NULL ? 1 : 0;

  if (send_and_destroy(client, read_response_create(result, count)) != 0) {
    log_debug("error sending response");
    return;
  }
}

int send_and_destroy(client_t *client, buffer_t packet) {
  return client_send_and_destroy_data(client, packet.content, packet.size);
}