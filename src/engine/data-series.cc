//
// Created by Pawel Burzynski on 28/01/2017.
//

#include <src/utils/stopwatch.h>
#include <cstdlib>
#include <src/utils/common.h>
#include <src/utils/file.h>
#include "data-series.h"
#include "data-point-reader.h"

#define A_PAGE_ALLOCATE_BUFFER_SIZE 65536

namespace apollo {

DataSeries::DataSeries(std::string file_name, int points_per_chunk, Log *log) {
  this->file_name = file_name;
  this->points_per_chunk = points_per_chunk;
  this->log = log;
}

DataSeries::~DataSeries() {
  this->log->Info("Deleting data series");

  for (auto chunk: this->chunks) {
    delete chunk;
  }

  this->chunks.clear();
}

DataSeries *DataSeries::Init(std::string file_name, int points_per_chunk, Log *log) {
  log->Info("Loading data series ...");
  Stopwatch sw;
  DataSeries *series = new DataSeries(file_name, points_per_chunk, log);
  File f(file_name);
  int chunk_size = DataChunk::CalculateChunkSize(points_per_chunk);

  sw.Start();

  for (int i = 0; i < f.GetSize() / chunk_size; i++) {
    DataChunk *chunk = DataChunk::Load(file_name, (uint64_t)i * chunk_size, points_per_chunk);
    series->RegisterChunk(chunk);
  }

  sw.Stop();
  log->Info("Database loaded in: " + std::to_string(sw.GetElapsedMilliseconds() / 1000) + "[s]");
  return series;
}

void DataSeries::Write(data_point_t *points, int count) {
  auto lock_scope = std::unique_ptr<RwLockScope>(this->series_lock.LockRead());
  lock_scope->UpgradeToWrite();

  if (this->chunks.size() == 0) {
    DataChunk *chunk = this->CreateEmptyChunk();
    this->RegisterChunk(chunk);
  }

  int first_current = 0;
  DataChunk *last_chunk = this->chunks.back();

  while (first_current < count && points[first_current].time <= last_chunk->GetEnd()) {
    first_current++;
  }

  this->WriteChunk(last_chunk, points + first_current, count - first_current);

  if (first_current != 0) {
    int start = 0;
    int stop = 0;

    for (auto chunk: this->chunks) {
      while (stop < first_current && points[stop].time <= chunk->GetEnd()) {
        stop++;
      }

      if (stop != start) {
        this->WriteChunk(chunk, points + start, stop - start);
      }

      start = stop;
    }
  }
}

std::shared_ptr<DataPointReader> DataSeries::Read(timestamp_t begin, timestamp_t end) {
  auto lock_scope = std::unique_ptr<RwLockScope>(this->series_lock.LockRead());
  std::list<DataChunk *> filtered_chunks;

  for (auto chunk: this->chunks) {
    if (chunk->GetBegin() < end && chunk->GetEnd() >= begin) {
      filtered_chunks.push_back(chunk);
    }
  }

  if (filtered_chunks.size() == 0) {
    return std::make_shared<DataPointReader>(nullptr, 0);
  }

  auto comp = [](data_point_t p, timestamp_t t) -> bool { return p.time < t; };

  data_point_t *front_begin = filtered_chunks.front()->Read();
  data_point_t *front_end = front_begin + filtered_chunks.front()->GetNumberOfPoints();

  data_point_t *back_begin = filtered_chunks.back()->Read();
  data_point_t *back_end = back_begin + filtered_chunks.back()->GetNumberOfPoints();

  data_point_t *read_begin = std::lower_bound(front_begin, front_end, begin, comp);
  data_point_t *read_end = std::lower_bound(back_begin, back_end, end, comp);

  if (filtered_chunks.size() == 1) {
    uint64_t total_points = read_end - read_begin;
    data_point_t *snapshot = (data_point_t *)calloc((size_t)total_points, sizeof(data_point_t));
    memcpy(snapshot, read_begin, total_points * sizeof(data_point_t));

    return std::make_shared<DataPointReader>(snapshot, total_points);
  }

  uint64_t total_points = 0;
  uint64_t points_from_front = front_end - read_begin;
  uint64_t points_from_back = read_end - back_begin;

  total_points += points_from_front;
  total_points += points_from_back;

  for (auto i: filtered_chunks) {
    if (i != filtered_chunks.front() && i != filtered_chunks.back()) {
      total_points += i->GetNumberOfPoints();
    }
  }

  data_point_t *snapshot = (data_point_t *)calloc((size_t)total_points, sizeof(data_point_t));
  int snapshot_position = 0;

  memcpy(snapshot, read_begin, points_from_front * sizeof(data_point_t));
  snapshot_position += points_from_front;

  for (auto i: filtered_chunks) {
    if (i != filtered_chunks.front() && i != filtered_chunks.back()) {
      memcpy(snapshot + snapshot_position, i->Read(), (size_t)i->GetNumberOfPoints() * sizeof(data_point_t));
      snapshot_position += i->GetNumberOfPoints();
    }
  }

  memcpy(snapshot + snapshot_position, back_begin, points_from_back * sizeof(data_point_t));

  return std::make_shared<DataPointReader>(snapshot, total_points);
}

void DataSeries::PrintMetadata() {
  for (auto chunk: this->chunks) {
    chunk->PrintMetadata();
  }
}

void DataSeries::RegisterChunk(DataChunk *chunk) {
  auto i = this->chunks.begin();

  while (i != this->chunks.end() && (
      ((*i)->GetBegin() < chunk->GetBegin()) ||
          ((*i)->GetBegin() == chunk->GetBegin() && (*i)->GetEnd() < chunk->GetEnd()))) {
    i++;
  }

  this->chunks.insert(i, chunk);
}

void DataSeries::WriteChunk(DataChunk *chunk, data_point_t *points, int count) {
  if (count == 0) {
    return;
  }

  if (chunk->GetEnd() <= points[0].time) {
    this->ChunkMemcpy(chunk, chunk->GetNumberOfPoints(), points, count);
  } else {
    int buffer_count = count + chunk->GetNumberOfPoints();
    data_point_t *buffer = (data_point_t *)calloc((size_t)buffer_count, sizeof(data_point_t));
    data_point_t *content = chunk->Read();
    int points_pos = count - 1;
    int content_pos = chunk->GetNumberOfPoints() - 1;

    for (int i = buffer_count - 1; i >= 0; i--) {
      if (points_pos < 0) {
        buffer[i] = content[content_pos--];
      } else if (content_pos < 0) {
        buffer[i] = points[points_pos--];
      } else if (points[points_pos].time < content[content_pos].time) {
        buffer[i] = content[content_pos--];
      } else {
        buffer[i] = points[points_pos--];
      }
    }

    this->ChunkMemcpy(chunk, 0, buffer, buffer_count);
    free(buffer);
  }
}

void DataSeries::ChunkMemcpy(DataChunk *chunk, int position, data_point_t *points, int count) {
  int to_write = MIN(count, chunk->GetMaxNumberOfPoints() - position);
  chunk->Write(position, points, to_write);
  count -= to_write;
  points += to_write;

  while (count != 0) {
    chunk = this->CreateEmptyChunk();
    to_write = MIN(count, chunk->GetMaxNumberOfPoints());
    chunk->Write(0, points, to_write);
    this->RegisterChunk(chunk);
    count -= to_write;
    points += to_write;
  }
}

DataChunk *DataSeries::CreateEmptyChunk() {

  uint8_t buffer[A_PAGE_ALLOCATE_BUFFER_SIZE] = {0};
  int to_allocate = DataChunk::CalculateChunkSize(this->points_per_chunk);

  File file(this->file_name);
  file.Seek(0, SEEK_END);

  while (to_allocate > 0) {
    int to_write = MIN(to_allocate, A_PAGE_ALLOCATE_BUFFER_SIZE);
    file.Write(buffer, (size_t)to_write);
    to_allocate -= to_write;
  }

  file.Flush();

  return DataChunk::Load(
      this->file_name,
      DataChunk::CalculateChunkSize(this->points_per_chunk) * this->chunks.size(),
      this->points_per_chunk);
}

}