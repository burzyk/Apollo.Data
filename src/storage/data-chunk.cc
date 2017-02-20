//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cmath>
#include <src/utils/common.h>
#include <src/log.h>
#include <cstdlib>
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
    chunk->begin = min(chunk->begin, points[i].time);
    chunk->end = max(chunk->end, points[i].time);
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

void DataChunk::PrintMetadata() {
  printf(
      "begin: %llu, end: %llu, points: %d\n",
      this->begin,
      this->end,
      this->number_of_points);

  data_point_t *points = this->Read();

  for (int i = 0; i < this->number_of_points; i++) {
    printf("    %llu %f\n", points[i].time, points[i].value);
  }
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

