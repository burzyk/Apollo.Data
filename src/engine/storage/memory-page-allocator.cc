//
// Created by Pawel Burzynski on 21/01/2017.
//

#include <cstdlib>
#include <src/utils/log.h>
#include <src/fatal-exception.h>
#include "memory-page-allocator.h"

namespace apollo {

MemoryPageAllocator::MemoryPageAllocator(size_t page_size, int max_pages) {
  this->page_size = page_size;
  this->max_pages = max_pages;
  this->next_page_id = 0;
}

MemoryPageAllocator::~MemoryPageAllocator() {
  for (int i = 0; i < this->pages.size(); i++) {
    this->DeallocateLastPage();
  }
}

uint8_t *MemoryPageAllocator::GetPage(int page_id) {
  auto page = this->pages.find(page_id);

  if (page == this->pages.end()) {
    return nullptr;
  } else {
    page->second->access_time = clock();
    return page->second->page;
  }
}

page_id_t MemoryPageAllocator::AllocatePage() {
  if (this->pages.size() >= this->max_pages) {
    this->DeallocateLastPage();
  }

  page_info_t *new_page = (page_info_t *)calloc(1, sizeof(page_info_t));

  if (new_page == nullptr) {
    throw FatalException("Unable to allocate page info");
  }

  new_page->page = (uint8_t *)calloc(this->page_size, 1);

  if (new_page->page == nullptr) {
    throw FatalException("Unable to allocate page");
  }

  new_page->page_id = this->next_page_id++;
  this->pages[new_page->page_id] = new_page;

  return new_page->page_id;
}

void MemoryPageAllocator::DeallocateLastPage() {
  page_info_t *last_page = nullptr;

  for (auto p: this->pages) {
    if (last_page == nullptr || last_page->access_time > p.second->access_time) {
      last_page = p.second;
    }
  }

  if (last_page == nullptr) {
    throw FatalException("Unable to find page to deallocate");
  }

  this->pages.erase(last_page->page_id);
  free(last_page->page);
  free(last_page);
}

}

