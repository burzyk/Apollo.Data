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

#include "src/storage/data-series.h"

#include <cstdlib>
#include <src/utils/memory.h>
#include <src/utils/diagnostics.h>

#include "src/utils/disk.h"
#include "src/utils/allocator.h"
#include "src/storage/standard-data-points-reader.h"

namespace shakadb {

DataSeries::DataSeries(std::string file_name, int points_per_chunk) {
  this->file_name = file_name;
  this->points_per_chunk = points_per_chunk;
  this->series_lock = sdb_rwlock_create();
}

DataSeries::~DataSeries() {
  this->DeleteChunks();
  sdb_rwlock_destroy(this->series_lock);
}

DataSeries *DataSeries::Init(std::string file_name, int points_per_chunk) {
  DataSeries *series = new DataSeries(file_name, points_per_chunk);
  int chunk_size = sdb_data_chunk_calculate_size(points_per_chunk);

  // sdb_stopwatch_t *sw = sdb_stopwatch_start();

  for (int i = 0; i < sdb_file_size(file_name.c_str()) / chunk_size; i++) {
    sdb_data_chunk_t *chunk = sdb_data_chunk_create(file_name.c_str(), (uint64_t)i * chunk_size, points_per_chunk);

    if (chunk != NULL) {
      series->RegisterChunk(chunk);
    } else {
      // TODO: (pburzynki): Improve logging : log->Info("Unable to load chunk");
    }
  }

  // log->Info("Data series loaded in: " + std::to_string(sdb_stopwatch_stop_and_destroy(sw)) + "[s]");
  return series;
}

void DataSeries::Write(sdb_data_point_t *points, int count) {
  sdb_rwlock_wrlock(this->series_lock);

  if (this->chunks.size() == 0) {
    sdb_data_chunk_t *chunk = this->CreateEmptyChunk();
    this->RegisterChunk(chunk);
  }

  int first_current = 0;
  sdb_data_chunk_t *last_chunk = this->chunks.back();

  while (first_current < count && points[first_current].time <= last_chunk->end) {
    first_current++;
  }

  this->WriteChunk(last_chunk, points + first_current, count - first_current);

  if (first_current != 0) {
    int start = 0;
    int stop = 0;

    for (auto chunk : this->chunks) {
      while (stop < first_current && points[stop].time <= chunk->end) {
        stop++;
      }

      if (stop != start) {
        this->WriteChunk(chunk, points + start, stop - start);
      }

      start = stop;
    }
  }

  sdb_rwlock_unlock(this->series_lock);
}

void DataSeries::Truncate() {
  sdb_rwlock_wrlock(this->series_lock);

  this->DeleteChunks();
  sdb_file_truncate(this->file_name.c_str());

  sdb_rwlock_unlock(this->series_lock);
}

sdb_data_points_reader_t *DataSeries::Read(timestamp_t begin, timestamp_t end, int max_points) {
  sdb_rwlock_rdlock(this->series_lock);
  std::list<sdb_data_chunk_t *> filtered_chunks;

  for (auto chunk : this->chunks) {
    if (chunk->begin < end && chunk->end >= begin) {
      filtered_chunks.push_back(chunk);
    }
  }

  if (filtered_chunks.size() == 0) {
    sdb_rwlock_unlock(this->series_lock);
    return sdb_data_points_reader_create(0);
  }

  auto comp = [](sdb_data_point_t p, timestamp_t t) -> bool { return p.time < t; };

  sdb_data_point_t *front_begin = sdb_data_chunk_read(filtered_chunks.front());
  sdb_data_point_t *front_end = front_begin + filtered_chunks.front()->number_of_points;

  sdb_data_point_t *back_begin = sdb_data_chunk_read(filtered_chunks.back());
  sdb_data_point_t *back_end = back_begin + filtered_chunks.back()->number_of_points;

  sdb_data_point_t *read_begin = std::lower_bound(front_begin, front_end, begin, comp);
  sdb_data_point_t *read_end = std::lower_bound(back_begin, back_end, end, comp);

  if (filtered_chunks.size() == 1) {
    int total_points = std::min(static_cast<int>(read_end - read_begin), max_points);
    sdb_data_points_reader_t *reader = sdb_data_points_reader_create(total_points);
    sdb_data_points_reader_write(reader, read_begin, total_points);

    sdb_rwlock_unlock(this->series_lock);
    return reader;
  }

  int total_points = 0;
  int points_from_front = front_end - read_begin;
  int points_from_back = read_end - back_begin;

  total_points += points_from_front;
  total_points += points_from_back;

  for (auto i : filtered_chunks) {
    if (i != filtered_chunks.front() && i != filtered_chunks.back()) {
      total_points += i->number_of_points;
    }
  }

  sdb_data_points_reader_t *reader = sdb_data_points_reader_create(std::min(total_points, max_points));

  if (!sdb_data_points_reader_write(reader, read_begin, points_from_front)) {
    sdb_rwlock_unlock(this->series_lock);
    return reader;
  }

  for (auto i : filtered_chunks) {
    if (i != filtered_chunks.front() && i != filtered_chunks.back()) {
      if (!sdb_data_points_reader_write(reader, sdb_data_chunk_read(i), i->number_of_points)) {
        sdb_rwlock_unlock(this->series_lock);
        return reader;
      }
    }
  }

  sdb_data_points_reader_write(reader, back_begin, points_from_back);

  sdb_rwlock_unlock(this->series_lock);
  return reader;
}

void DataSeries::RegisterChunk(sdb_data_chunk_t *chunk) {
  auto i = this->chunks.begin();

  // TODO: (pburzynski) can be optimized
  while (i != this->chunks.end() && (
      ((*i)->begin < chunk->begin) ||
          ((*i)->begin == chunk->begin
              && (*i)->end < chunk->end))) {
    i++;
  }

  this->chunks.insert(i, chunk);
}

void DataSeries::WriteChunk(sdb_data_chunk_t *chunk, sdb_data_point_t *points, int count) {
  if (count == 0) {
    return;
  }

  if (chunk->end < points[0].time) {
    this->ChunkMemcpy(chunk, chunk->number_of_points, points, count);
  } else {
    int buffer_count = count + chunk->number_of_points;
    sdb_data_point_t *buffer = (sdb_data_point_t *)sdb_alloc(buffer_count * sizeof(sdb_data_point_t));
    sdb_data_point_t *content = sdb_data_chunk_read(chunk);
    int points_pos = count - 1;
    int content_pos = chunk->number_of_points - 1;
    int duplicated_count = 0;

    for (int i = buffer_count - 1; i >= duplicated_count; i--) {
      if (points_pos < 0) {
        buffer[i] = content[content_pos--];
      } else if (content_pos < 0) {
        buffer[i] = points[points_pos--];
      } else if (points[points_pos].time < content[content_pos].time) {
        buffer[i] = content[content_pos--];
      } else if (points[points_pos].time == content[content_pos].time) {
        buffer[i] = points[points_pos--];
        content_pos--;
        duplicated_count++;
      } else {
        buffer[i] = points[points_pos--];
      }
    }

    this->ChunkMemcpy(chunk, 0, buffer + duplicated_count, buffer_count - duplicated_count);
    Allocator::Delete(buffer);
  }
}

void DataSeries::ChunkMemcpy(sdb_data_chunk_t *chunk, int position, sdb_data_point_t *points, int count) {
  int to_write = std::min(count, chunk->max_points - position);
  sdb_data_chunk_write(chunk, position, points, to_write);
  count -= to_write;
  points += to_write;

  while (count != 0) {
    chunk = this->CreateEmptyChunk();
    to_write = std::min(count, chunk->max_points);
    sdb_data_chunk_write(chunk, 0, points, to_write);
    this->RegisterChunk(chunk);
    count -= to_write;
    points += to_write;
  }
}

sdb_data_chunk_t *DataSeries::CreateEmptyChunk() {
  int buffer_size = this->points_per_chunk / 2;
  void *buffer = sdb_alloc(buffer_size);
  int to_allocate = sdb_data_chunk_calculate_size(this->points_per_chunk);

  sdb_file_t *file = sdb_file_open(this->file_name.c_str());
  sdb_file_seek(file, 0, SEEK_END);

  while (to_allocate > 0) {
    int to_write = std::min(to_allocate, buffer_size);
    sdb_file_write(file, buffer, (size_t)to_write);
    to_allocate -= to_write;
  }

  Allocator::Delete(buffer);
  sdb_file_close(file);

  return sdb_data_chunk_create(
      this->file_name.c_str(),
      sdb_data_chunk_calculate_size(this->points_per_chunk) * this->chunks.size(),
      this->points_per_chunk);
}

void DataSeries::DeleteChunks() {
  for (auto chunk : this->chunks) {
    sdb_data_chunk_destroy(chunk);
  }

  this->chunks.clear();
}

}  // namespace shakadb
