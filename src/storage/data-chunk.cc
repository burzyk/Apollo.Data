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

#include <cmath>
#include <src/utils/common.h>
#include <src/log.h>
#include <cstdlib>
#include <string.h>
#include <src/utils/file.h>
#include <src/fatal-exception.h>
#include <src/utils/allocator.h>
#include "data-chunk.h"

namespace shakadb {

DataChunk::~DataChunk() {
  if (this->cached_content != nullptr) {
    Allocator::Delete(this->cached_content);
    this->cached_content = nullptr;
  }
}

DataChunk *DataChunk::Load(std::string file_name, uint64_t file_offset, int max_points) {
  DataChunk *chunk = new DataChunk(file_name, file_offset, max_points);
  data_point_t *points = Allocator::New<data_point_t>(max_points);

  File f(file_name);

  f.Seek(file_offset, SEEK_SET);
  f.Read(points, max_points * sizeof(data_point_t));

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
  auto scope = this->lock.LockRead();

  if (this->cached_content == nullptr) {
    scope->UpgradeToWrite();

    if (this->cached_content == nullptr) {
      this->cached_content = Allocator::New<data_point_t>(this->max_points);

      File f(this->file_name);
      f.Seek(this->file_offset, SEEK_SET);
      f.Read(this->cached_content, this->max_points * sizeof(data_point_t));
    }
  }

  return this->cached_content;
}

void DataChunk::Write(int offset, data_point_t *points, int count) {
  auto scope = this->lock.LockWrite();

  if (count == 0) {
    return;
  }

  if (this->max_points < offset + count) {
    throw FatalException("Trying to write outside data chunk");
  }

  File f(this->file_name);
  f.Seek(this->file_offset + offset * sizeof(data_point_t), SEEK_SET);
  f.Write(points, count * sizeof(data_point_t));

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
}

}

