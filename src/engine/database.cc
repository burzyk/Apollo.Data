//
// Created by Pawel Burzynski on 17/01/2017.
//

#include <cstdlib>
#include <cmath>
#include <src/utils/common.h>
#include "database.h"

namespace apollo {

Database::Database(Storage *storage) {
  this->storage = storage;
  this->chunks_count = 0;
}

Database::~Database() {
  for (auto s: this->series) {
    for (auto chunk : *s.second) {
      delete chunk;
    }

    delete s.second;
  }

  this->series.clear();
  this->storage = NULL;
}

Database *Database::Init(Storage *storage) {
  Database *db = new Database(storage);

  for (int i = 0; i < db->storage->GetPagesCount(); i++) {
    StoragePage *page = db->storage->GetPage(i);
    DataChunk *chunk = DataChunk::Load(page);

    if (chunk != NULL) {
      db->RegisterChunk(chunk);
    }
  }

  return db;
}

int Database::CalculatePageSize(int number_of_points) {
  return sizeof(apollo::data_chunk_info_t) + number_of_points * sizeof(apollo::data_point_t);
}

void Database::Write(std::string name, data_point_t *points, int count) {
  std::list<DataChunk *> *chunks = this->FindDataChunks(name);

  if (chunks->size() == 0) {
    DataChunk *chunk = DataChunk::Create(name, this->storage->AllocatePage());
    this->RegisterChunk(chunk);
  }

  int first_current = 0;
  DataChunk *last_chunk = chunks->back();

  while (first_current < count && points[first_current].time <= last_chunk->GetEnd()) {
    first_current++;
  }

  this->WriteChunk(last_chunk, points + first_current, count - first_current);

  if (first_current != 0) {
    int start = 0;
    int stop = 0;

    for (auto chunk: *chunks) {
      while (stop < first_current && points[stop].time <= chunk->GetEnd()) {
        stop++;
      }

      this->WriteChunk(chunk, points + start, stop - start);
      start = stop;
    }
  }

  this->storage->Flush();
}

DataPointReader Database::Read(std::string name, timestamp_t begin, timestamp_t end) {
  std::list<DataChunk *> *chunks = this->FindDataChunks(name);
  std::list<DataChunk *> filtered_chunks;

  for (auto chunk: *chunks) {
    if (chunk->GetBegin() <= end && chunk->GetEnd() >= begin) {
      filtered_chunks.push_back(chunk);
    }
  }

  return DataPointReader(filtered_chunks, begin, end);
}

void Database::PrintMetadata() {
  printf("Database:\n");
  printf("    chunks count: %llu\n", this->chunks_count);

  for (auto series: this->series) {
    printf("==================================================\n");
    printf("Series: %s\n", series.first.c_str());

    for (auto chunk: *series.second) {
      chunk->PrintMetadata();
    }
  }
}

void Database::RegisterChunk(DataChunk *chunk) {
  std::list<DataChunk *> *chunks = this->FindDataChunks(chunk->GetSeriesName());
  auto i = chunks->begin();

  while (i != chunks->end() && (
      ((*i)->GetBegin() < chunk->GetBegin()) ||
          ((*i)->GetBegin() == chunk->GetBegin() && (*i)->GetEnd() < chunk->GetEnd()))) {
    i++;
  }

  chunks->insert(i, chunk);
  this->chunks_count++;
}

std::list<DataChunk *> *Database::FindDataChunks(std::string name) {
  if (this->series.find(name) == this->series.end()) {
    this->series[name] = new std::list<DataChunk *>();
  }

  return this->series[name];
}

void Database::WriteChunk(DataChunk *chunk, data_point_t *points, int count) {
  if (count == 0) {
    return;
  }

  if (chunk->GetEnd() <= points[0].time) {
    this->ChunkMemcpy(chunk, chunk->GetNumberOfPoints(), points, count);
  } else {
    int buffer_count = count + chunk->GetNumberOfPoints();
    data_point_t *buffer = (data_point_t *)calloc((size_t)buffer_count, sizeof(data_point_t));
    data_point_t *content = chunk->Read();
    int points_pos = count - 1;
    int content_pos = chunk->GetNumberOfPoints() - 1;

    for (int i = buffer_count - 1; i >= 0; i--) {
      if (points_pos < 0) {
        buffer[i] = content[content_pos--];
      } else if (content_pos < 0) {
        buffer[i] = content[points_pos--];
      } else if (points[points_pos].time < content[content_pos].time) {
        buffer[i] = content[content_pos--];
      } else {
        buffer[i] = points[points_pos--];
      }
    }

    this->ChunkMemcpy(chunk, 0, buffer, buffer_count);
    free(buffer);
  }
}

void Database::ChunkMemcpy(DataChunk *chunk, int position, data_point_t *points, int count) {
  int to_write = MIN(count, chunk->GetMaxNumberOfPoints() - position);
  chunk->Write(position, points, to_write);
  count -= to_write;
  points += to_write;

  while (count != 0) {
    chunk = DataChunk::Create(chunk->GetSeriesName(),
                              this->storage->AllocatePage());
    to_write = MIN(count, chunk->GetMaxNumberOfPoints());
    chunk->Write(0, points, to_write);
    this->RegisterChunk(chunk);
    count -= to_write;
    points += to_write;
  }
}

}