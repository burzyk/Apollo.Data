//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <src/utils/common.h>
#include "data-point-reader.h"

namespace apollo {

DataPointReader::DataPointReader(std::list<DataChunk *> chunks,
                                 timestamp_t begin,
                                 timestamp_t end,
                                 std::shared_ptr<RwLockScope> lock_scope) {
  this->begin = begin;
  this->end = end;
  this->position = 0;
  this->chunks = std::vector<DataChunk *>(chunks.begin(), chunks.end());
  this->current_chunk = this->chunks.begin();
  this->lock_scope = lock_scope;
}

int DataPointReader::Read(apollo::data_point_t *buffer, int size) {

  return 0;
//  if (size == 0) {
//    return 0;
//  }
//
//  auto comp = [](data_point_t p, timestamp_t t) -> bool { return p.time < t; };
//  int total_read = 0;
//
//  while (size > 0 &&
//      this->current_chunk != this->chunks.end() &&
//      (*this->current_chunk)->Read()[this->position].time < this->end) {
//
//    data_point_t *all_points_begin = (*this->current_chunk)->Read();
//    data_point_t *all_points_end = all_points_begin + (*this->current_chunk)->GetNumberOfPoints();
//    data_point_t *start = this->position != 0
//                          ? all_points_begin + this->position
//                          : this->begin <= (*this->current_chunk)->GetBegin()
//                            ? all_points_begin
//                            : std::lower_bound(all_points_begin, all_points_end, this->begin, comp);
//    data_point_t *stop = this->end > (*this->current_chunk)->GetEnd()
//                         ? all_points_end
//                         : std::lower_bound(all_points_begin, all_points_end, this->end, comp);
//
//    if (this->position == 0) {
//      this->position = start - all_points_begin;
//    }
//
//    size_t to_copy = MIN(size, stop - start);
//    memcpy(buffer, start, to_copy * sizeof(data_point_t));
//    size -= to_copy;
//    buffer += to_copy;
//    this->position += to_copy;
//    total_read += to_copy;
//
//    if (this->position >= (*this->current_chunk)->GetNumberOfPoints()) {
//      this->position = 0;
//      this->current_chunk++;
//    }
//  }
//
//  return total_read;
}

}
