//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <utils/common.h>
#include "data-point-reader.h"

namespace apollo {

DataPointReader::DataPointReader(std::list<DataChunk *> chunks, timestamp_t begin, timestamp_t end) {
  this->begin = begin;
  this->end = end;
  this->position = 0;
  this->chunks = chunks;
  this->current_chunk = chunks.begin();
}

int DataPointReader::Read(apollo::data_point_t *buffer, int size) {
  auto upper_comp = [](timestamp_t t, data_point_t p) -> bool { return t < p.time; };
  auto lower_comp = [](data_point_t p, timestamp_t t) -> bool { return p.time < t; };
  int total_read = 0;

  while (size > 0 && this->current_chunk != this->chunks.end()) {
    data_point_t *all_points_begin = (*this->current_chunk)->Read();
    data_point_t *all_points_end = all_points_begin + (*this->current_chunk)->GetNumberOfPoints();
    data_point_t *start = this->begin <= (*this->current_chunk)->GetBegin()
                          ? all_points_begin
                          : std::lower_bound(all_points_begin, all_points_end, this->begin, lower_comp);
    data_point_t *stop = this->end >= (*this->current_chunk)->GetEnd()
                         ? all_points_end
                         : std::upper_bound(all_points_begin, all_points_end, this->end, upper_comp);

    if (this->position != 0) {
      start = all_points_begin + this->position;
    }

    size_t to_copy = MIN(size, stop - start);
    memcpy(buffer, start, to_copy);
    size -= to_copy;
    buffer += to_copy;
    this->position += to_copy;
    total_read += to_copy;

    if (this->position >= (*this->current_chunk)->GetNumberOfPoints()) {
      this->position = 0;
      this->current_chunk++;
    }
  }

  return total_read;
}

}
