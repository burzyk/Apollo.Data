//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_DATACHUNK_H
#define APOLLO_STORAGE_DATACHUNK_H

#include <string>
#include <src/data-point.h>

namespace apollo {

class DataChunk {
 public:
  ~DataChunk();
  static DataChunk *Load(std::string file_name, uint64_t file_offset, int max_points);
  static int CalculateChunkSize(int points);

  data_point_t *Read();
  void Write(int offset, data_point_t *points, int count);

  timestamp_t GetBegin();
  timestamp_t GetEnd();
  int GetNumberOfPoints();
  int GetMaxNumberOfPoints();

  void PrintMetadata();
 private:
  DataChunk(std::string file_name, uint64_t file_offset, int max_points);

  std::string file_name;
  uint64_t file_offset;
  int max_points;
  data_point_t *cached_content;

  timestamp_t begin;
  timestamp_t end;
  int number_of_points;
};

}

#endif //APOLLO_STORAGE_DATACHUNK_H
