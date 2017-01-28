//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cmath>
#include <src/utils/common.h>
#include <src/utils/log.h>
#include <cstdlib>
#include "data-chunk.h"

namespace apollo {

DataChunk *DataChunk::Create(StoragePage *page) {
  return new DataChunk(page);
}

DataChunk *DataChunk::Load(StoragePage *page) {
  DataChunk *chunk = new DataChunk(page);
  data_point_t *points = (data_point_t *)calloc((size_t)chunk->GetMaxNumberOfPoints(), sizeof(data_point_t));

  page->Read(0, points, page->GetPageSize());

  for (int i = 0; i < chunk->GetMaxNumberOfPoints() && points[i].time != 0; i++) {
    chunk->begin = MIN(chunk->begin, points[i].time);
    chunk->end = MAX(chunk->end, points[i].time);
    chunk->number_of_points++;
  }

  free(points);
  return chunk;
}

int DataChunk::Read(int offset, data_point_t *points, int count) {
  int read = this->page->Read(offset * sizeof(data_point_t), points, count * sizeof(data_point_t));
  return read / sizeof(data_point_t);
}

void DataChunk::Write(int offset, data_point_t *points, int count) {
  if (count == 0) {
    return;
  }

  this->page->Write(sizeof(data_point_t) * offset, points, sizeof(data_point_t) * count);

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

void DataChunk::PrintMetadata() {
  printf(
      "begin: %llu, end: %llu, points: %d\n",
      this->begin,
      this->end,
      this->number_of_points);

  data_point_t *points = (data_point_t *)calloc((size_t)this->number_of_points, sizeof(data_point_t));

  for (int i = 0; i < this->number_of_points; i++) {
    printf("    %llu %f\n", points[i].time, points[i].value);
  }

  free(points);
}

int DataChunk::GetMaxNumberOfPoints() {
  return this->page->GetPageSize() / sizeof(data_point_t);
}

DataChunk::DataChunk(StoragePage *page) {
  this->page = page;
  this->begin = A_MAX_TIMESTAMP;
  this->end = A_MIN_TIMESTAMP;
  this->number_of_points = 0;
}

}

