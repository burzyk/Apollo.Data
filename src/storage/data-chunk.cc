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
// Created by Pawel Burzynski on 17/01/2017.
//

#include "src/storage/data-chunk.h"

#include <string.h>
#include <cmath>
#include <cstdlib>
#include <algorithm>

#include "src/utils/common.h"
#include "src/log.h"
#include "src/fatal-exception.h"
#include "src/utils/allocator.h"
#include "src/utils/disk.h"

namespace shakadb {

DataChunk::~DataChunk() {
  if (this->cached_content != nullptr) {
    Allocator::Delete(this->cached_content);
    this->cached_content = nullptr;
  }

  sdb_rwlock_destroy(this->lock);
}

DataChunk *DataChunk::Load(std::string file_name, uint64_t file_offset, int max_points) {
  DataChunk *chunk = new DataChunk(file_name, file_offset, max_points);
  data_point_t *points = Allocator::New<data_point_t>(max_points);

  sdb_file_t *file = sdb_file_open(file_name.c_str());

  if (file == NULL) {
    return NULL;
  }

  sdb_file_seek(file, file_offset, SEEK_SET);
  sdb_file_read(file, points, max_points * sizeof(data_point_t));
  sdb_file_close(file);

  for (int i = 0; i < max_points && points[i].time != 0; i++) {
    chunk->begin = std::min(chunk->begin, points[i].time);
    chunk->end = std::max(chunk->end, points[i].time);
    chunk->number_of_points++;
  }

  Allocator::Delete(points);
  return chunk;
}

int DataChunk::CalculateChunkSize(int points) {
  return points * sizeof(data_point_t);
}

data_point_t *DataChunk::Read() {
  sdb_rwlock_rdlock(this->lock);

  if (this->cached_content == nullptr) {
    sdb_rwlock_upgrade(this->lock);

    if (this->cached_content == nullptr) {
      this->cached_content = Allocator::New<data_point_t>(this->max_points);

      sdb_file_t *file = sdb_file_open(this->file_name.c_str());
      sdb_file_seek(file, this->file_offset, SEEK_SET);
      sdb_file_read(file, this->cached_content, this->max_points * sizeof(data_point_t));
      sdb_file_close(file);
    }

    sdb_rwlock_unlock(this->lock);
  }

  return this->cached_content;
}

void DataChunk::Write(int offset, data_point_t *points, int count) {
  if (count == 0) {
    return;
  }

  sdb_rwlock_wrlock(this->lock);

  if (this->max_points < offset + count) {
    throw FatalException("Trying to write outside data chunk");
  }

  sdb_file_t *file = sdb_file_open(this->file_name.c_str());
  sdb_file_seek(file, this->file_offset + offset * sizeof(data_point_t), SEEK_SET);
  sdb_file_write(file, points, count * sizeof(data_point_t));
  sdb_file_close(file);

  if (this->cached_content != nullptr) {
    memcpy(this->cached_content + offset, points, count * sizeof(data_point_t));
  }

  if (offset == 0) {
    this->begin = points[0].time;
  }

  if (this->number_of_points <= offset + count) {
    this->end = points[count - 1].time;
    this->number_of_points = offset + count;
  }

  sdb_rwlock_unlock(this->lock);
}

timestamp_t DataChunk::GetBegin() {
  return this->begin;
}

timestamp_t DataChunk::GetEnd() {
  return this->end;
}

int DataChunk::GetNumberOfPoints() {
  return this->number_of_points;
}

int DataChunk::GetMaxNumberOfPoints() {
  return this->max_points;
}

DataChunk::DataChunk(std::string file_name, uint64_t file_offset, int max_points) {
  this->file_name = file_name;
  this->file_offset = file_offset;
  this->max_points = max_points;
  this->cached_content = nullptr;
  this->begin = data_point_t::kMaxTimestamp;
  this->end = data_point_t::kMinTimestamp;
  this->number_of_points = 0;
  this->lock = sdb_rwlock_create();
}

}  // namespace shakadb


