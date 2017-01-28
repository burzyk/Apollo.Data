//
// Created by Pawel Burzynski on 28/01/2017.
//

#include <src/utils/stopwatch.h>
#include <cstdlib>
#include <src/utils/common.h>
#include "data-series.h"
#include "data-point-reader.h"

namespace apollo {

DataSeries::DataSeries(Storage *storage, Log *log) {
  this->storage = storage;
  this->log = log;
}

DataSeries::~DataSeries() {
  this->log->Info("Deleting data series");

  for (auto chunk: this->chunks) {
    delete chunk;
  }

  this->chunks.clear();
  this->storage = nullptr;
}

DataSeries *DataSeries::Init(Storage *storage, Log *log) {
  log->Info("Loading database ...");
  Stopwatch sw;
  DataSeries *series = new DataSeries(storage, log);

  log->Info("Loading " + std::to_string(series->storage->GetPagesCount()) + " pages");
  sw.Start();

  for (int i = 0; i < series->storage->GetPagesCount(); i++) {
    StoragePage *page = series->storage->GetPage(i);
    DataChunk *chunk = DataChunk::Load(page);

    if (chunk != nullptr) {
      log->Debug("Loading chunk: " + std::to_string(i));
      series->RegisterChunk(chunk);
    }
  }

  sw.Stop();
  log->Info("Database loaded in: " + std::to_string(sw.GetElapsedMilliseconds() / 1000) + "[s]");
  return series;
}

void DataSeries::Write(data_point_t *points, int count) {
  if (this->chunks.size() == 0) {
    DataChunk *chunk = DataChunk::Create(this->storage->AllocatePage());
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

DataPointReader DataSeries::Read(timestamp_t begin, timestamp_t end) {

  // TODO: refactor!!!!!

//  auto lock = std::shared_ptr<RwLockScope>(this->master_lock->LockRead());
//  std::list<DataChunk *> *chunks = this->FindDataChunks(name);
//  std::list<DataChunk *> filtered_chunks;
//
//  for (auto chunk: *chunks) {
//    if (chunk->GetBegin() <= end && chunk->GetEnd() >= begin) {
//      filtered_chunks.push_back(chunk);
//    }
//  }
//
//  return DataPointReader(filtered_chunks, begin, end, lock);
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
    data_point_t *content = buffer;
    chunk->Read(0, buffer, chunk->GetNumberOfPoints());
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
    chunk = DataChunk::Create(this->storage->AllocatePage());
    to_write = MIN(count, chunk->GetMaxNumberOfPoints());
    chunk->Write(0, points, to_write);
    this->RegisterChunk(chunk);
    count -= to_write;
    points += to_write;
  }
}

}