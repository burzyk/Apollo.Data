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
// Created by Pawel Burzynski on 28/01/2017.
//

#include <src/utils/stopwatch.h>
#include <cstdlib>
#include <algorithm>
#include <src/utils/common.h>
#include <src/utils/file.h>
#include <src/utils/allocator.h>
#include "data-series.h"
#include "standard-data-points-reader.h"

namespace shakadb {

DataSeries::DataSeries(std::string file_name, int points_per_chunk, Log *log) {
  this->file_name = file_name;
  this->points_per_chunk = points_per_chunk;
  this->log = log;
}

DataSeries::~DataSeries() {
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
  log->Info("Data series loaded in: " + std::to_string(sw.GetElapsedSeconds()) + "[s]");
  return series;
}

void DataSeries::Write(data_point_t *points, int count) {
  auto lock_scope = this->series_lock.LockWrite();

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

DataPointsReader *DataSeries::Read(timestamp_t begin, timestamp_t end, int max_points) {
  auto lock_scope = this->series_lock.LockRead();
  std::list<DataChunk *> filtered_chunks;

  for (auto chunk: this->chunks) {
    if (chunk->GetBegin() < end && chunk->GetEnd() >= begin) {
      filtered_chunks.push_back(chunk);
    }
  }

  if (filtered_chunks.size() == 0) {
    return new StandardDataPointsReader(0);
  }

  auto comp = [](data_point_t p, timestamp_t t) -> bool { return p.time < t; };

  data_point_t *front_begin = filtered_chunks.front()->Read();
  data_point_t *front_end = front_begin + filtered_chunks.front()->GetNumberOfPoints();

  data_point_t *back_begin = filtered_chunks.back()->Read();
  data_point_t *back_end = back_begin + filtered_chunks.back()->GetNumberOfPoints();

  data_point_t *read_begin = std::lower_bound(front_begin, front_end, begin, comp);
  data_point_t *read_end = std::lower_bound(back_begin, back_end, end, comp);

  if (filtered_chunks.size() == 1) {
    int total_points = std::min((int)(read_end - read_begin), max_points);
    StandardDataPointsReader *reader = new StandardDataPointsReader(total_points);
    reader->WriteDataPoints(read_begin, total_points);

    return reader;
  }

  int total_points = 0;
  int points_from_front = front_end - read_begin;
  int points_from_back = read_end - back_begin;

  total_points += points_from_front;
  total_points += points_from_back;

  for (auto i: filtered_chunks) {
    if (i != filtered_chunks.front() && i != filtered_chunks.back()) {
      total_points += i->GetNumberOfPoints();
    }
  }

  StandardDataPointsReader *reader = new StandardDataPointsReader(std::min(total_points, max_points));

  if (!reader->WriteDataPoints(read_begin, points_from_front)) {
    return reader;
  }

  for (auto i: filtered_chunks) {
    if (i != filtered_chunks.front() && i != filtered_chunks.back()) {
      if (!reader->WriteDataPoints(i->Read(), i->GetNumberOfPoints())) {
        return reader;
      }
    }
  }

  reader->WriteDataPoints(back_begin, points_from_back);

  return reader;
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
    data_point_t *buffer = Allocator::New<data_point_t>(buffer_count);
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
    Allocator::Delete(buffer);
  }
}

void DataSeries::ChunkMemcpy(DataChunk *chunk, int position, data_point_t *points, int count) {
  int to_write = std::min(count, chunk->GetMaxNumberOfPoints() - position);
  chunk->Write(position, points, to_write);
  count -= to_write;
  points += to_write;

  while (count != 0) {
    chunk = this->CreateEmptyChunk();
    to_write = std::min(count, chunk->GetMaxNumberOfPoints());
    chunk->Write(0, points, to_write);
    this->RegisterChunk(chunk);
    count -= to_write;
    points += to_write;
  }
}

DataChunk *DataSeries::CreateEmptyChunk() {
  int buffer_size = this->points_per_chunk / 2;
  byte_t *buffer = Allocator::New<byte_t>(buffer_size);
  int to_allocate = DataChunk::CalculateChunkSize(this->points_per_chunk);

  File file(this->file_name);
  file.Seek(0, SEEK_END);

  while (to_allocate > 0) {
    int to_write = std::min(to_allocate, buffer_size);
    file.Write(buffer, (size_t)to_write);
    to_allocate -= to_write;
  }

  Allocator::Delete(buffer);
  file.Flush();

  return DataChunk::Load(
      this->file_name,
      DataChunk::CalculateChunkSize(this->points_per_chunk) * this->chunks.size(),
      this->points_per_chunk);
}

}
