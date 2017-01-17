//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cmath>
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
  memcpy(&info, page->Read(0, sizeof(data_chunk_info_t)), sizeof(data_chunk_info_t));

  if (strlen(info.series_name) == 0) {
    return NULL;
  }

  DataChunk *chunk = new DataChunk(std::string(info.series_name), page);
  data_point_t *points = chunk->Read(0, chunk->GetMaxNumberOfPoints());

  for (int i = 0; i < chunk->GetMaxNumberOfPoints() && points[i].time != 0; i++) {
    chunk->begin_ = fmin(chunk->begin_, points[i].time);
    chunk->end_ = fmax(chunk->end_, points[i].time);
    chunk->number_of_points_++;
  }

  return chunk;
}

data_point_t *DataChunk::Read(uint64_t offset, uint64_t count) {
  return (data_point_t *)
      this->page_->Read(
          sizeof(data_chunk_info_t) + sizeof(data_point_t) * offset,
          sizeof(data_point_t) * count);
}

void DataChunk::Write(uint64_t offset, data_point_t *points, uint64_t count) {
  this->page_->Write(
      sizeof(data_chunk_info_t) + sizeof(data_point_t) * offset,
      points,
      sizeof(data_point_t) * count);

  this->begin_ = fmin(this->begin_, points[0].time);
  this->end_ = fmax(this->end_, points[count - 1].time);
  this->number_of_points_ += count;
}

std::string DataChunk::GetSeriesName() {
  return this->series_name_;
}

timestamp_t DataChunk::GetBegin() {
  return this->begin_;
}

timestamp_t DataChunk::GetEnd() {
  return this->end_;
}

uint64_t DataChunk::GetNumberOfPoints() {
  return this->number_of_points_;
}

void DataChunk::PrintMetadata() {
  printf(
      "begin: %llu, end: %llu, points: %llu\n",
      this->begin_,
      this->end_,
      this->number_of_points_);
}

int DataChunk::GetMaxNumberOfPoints() {
  return (this->page_->GetPageSize() - sizeof(data_chunk_info_t)) / sizeof(data_point_t);
}

DataChunk::DataChunk(std::string series_name, StoragePage *page) {
  this->page_ = page;
  this->series_name_ = series_name;
  this->begin_ = A_MAX_TIMESTAMP;
  this->end_ = A_MIN_TIMESTAMP;
  this->number_of_points_ = 0;
}

}

