//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cmath>
#include <src/utils/common.h>
#include <src/utils/log.h>
#include <cstdlib>
#include "data-chunk.h"

namespace apollo {

DataChunk *DataChunk::Create(std::string series_name, StoragePage *page) {
  data_chunk_info_t info;
  memset(&info, 0, sizeof(data_chunk_info_t));
  strcpy(info.series_name, series_name.c_str());
  page->Write(0, &info, sizeof(data_chunk_info_t));

  return new DataChunk(series_name, page);
}

DataChunk *DataChunk::Load(StoragePage *page) {
  data_chunk_info_t info;
  page->Read(0, &info, sizeof(data_chunk_info_t));

  if (strlen(info.series_name) == 0) {
    return nullptr;
  }

  DataChunk *chunk = new DataChunk(std::string(info.series_name), page);
  std::shared_ptr<data_point_t> points = chunk->Read(chunk->GetMaxNumberOfPoints());

  for (int i = 0; i < chunk->GetMaxNumberOfPoints() && points.get()[i].time != 0; i++) {
    chunk->begin = MIN(chunk->begin, points.get()[i].time);
    chunk->end = MAX(chunk->end, points.get()[i].time);
    chunk->number_of_points++;
  }

  return chunk;
}

int DataChunk::Read(int offset, data_point_t *points, int count) {
  std::shared_ptr<data_point_t> buffer = this->Read(this->number_of_points);

  memcpy(points, buffer.get() + offset, sizeof(data_point_t) * count);

  return this->number_of_points;
}

void DataChunk::Write(int offset, data_point_t *points, int count) {
  if (count == 0) {
    return;
  }

  this->page->Write(sizeof(data_chunk_info_t) + sizeof(data_point_t) * offset,
                    points,
                    sizeof(data_point_t) * count);

  if (offset == 0) {
    this->begin = points[0].time;
  }

  if (this->number_of_points <= offset + count) {
    this->end = points[count - 1].time;
    this->number_of_points = offset + count;
  }
}

std::string DataChunk::GetSeriesName() {
  return this->series_name;
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

void DataChunk::PrintMetadata() {
  printf(
      "begin: %llu, end: %llu, points: %d (%s)\n",
      this->begin,
      this->end,
      this->number_of_points,
      this->series_name.c_str());

  std::shared_ptr<data_point_t> points = this->Read(this->GetNumberOfPoints());

  for (int i = 0; i < this->GetNumberOfPoints(); i++) {
    printf("    %llu %f\n", points.get()[i].time, points.get()[i].value);
  }
}

int DataChunk::GetMaxNumberOfPoints() {
  return (this->page->GetPageSize() - sizeof(data_chunk_info_t)) / sizeof(data_point_t);
}

DataChunk::DataChunk(std::string series_name, StoragePage *page) {
  this->page = page;
  this->series_name = series_name;
  this->begin = A_MAX_TIMESTAMP;
  this->end = A_MIN_TIMESTAMP;
  this->number_of_points = 0;
}

std::shared_ptr<data_point_t> DataChunk::Read(int count) {
  size_t buffer_size = sizeof(data_point_t) * count;
  uint8_t *buffer = (uint8_t *)calloc(buffer_size, 1);
  this->page->Read(sizeof(data_chunk_info_t), buffer, (int)buffer_size);

  return std::shared_ptr<data_point_t>((data_point_t *)buffer);
}

}

