//
// Created by Pawel Burzynski on 17/01/2017.
//

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
  if (this->current_chunk == this->chunks.end()) {
    return 0;
  }

  auto upper_comp = [](timestamp_t t, data_point_t p) -> bool { return t < p.time; };
  auto lower_comp = [](data_point_t p, timestamp_t t) -> bool { return p.time < t; };

  while (size > 0) {
    data_point_t *all_points_begin = (*this->current_chunk)->Read();
    data_point_t *all_points_end = all_points_begin + (*this->current_chunk)->GetNumberOfPoints();
    data_point_t *start = std::lower_bound(all_points_begin, all_points_end, this->begin, lower_comp);
    data_point_t *stop = std::upper_bound(all_points_begin, all_points_end, this->end, upper_comp);

  }
}

}
