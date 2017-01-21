//
// Created by Pawel Burzynski on 21/01/2017.
//

#include <cstdlib>
#include <src/utils/log.h>
#include "page-allocator.h"

namespace apollo {

PageAllocator::PageAllocator(int page_size, int max_pages) {
  this->page_size = page_size;
  this->max_pages = max_pages;
  this->next_page_id = 100;
}

PageAllocator::~PageAllocator() {
  for (int i = 0; i < this->pages.size(); i++) {
    this->DeallocateLastPage();
  }
}

uint8_t *PageAllocator::GetPage(int page_id) {
  auto page = this->pages.find(page_id);

  if (page == this->pages.end()) {
    return NULL;
  } else {
    page->second->access_time = clock();
    return page->second->page;
  }
}

page_id_t PageAllocator::AllocatePage() {
  if (this->pages.size() >= this->max_pages) {
    this->DeallocateLastPage();
  }

  page_info_t *new_page = (page_info_t *)calloc(1, sizeof(page_info_t));

  if (new_page == NULL) {
    Log::Fatal("Unable to allocate page info");
  }

  new_page->page = (uint8_t *)calloc(this->page_size, 1);

  if (new_page->page == NULL) {
    Log::Fatal("Unable to allocate page");
  }

  new_page->page_id = this->next_page_id++;
  this->pages[new_page->page_id] = new_page;

  return new_page->page_id;
}

void PageAllocator::DeallocateLastPage() {
  page_info_t *last_page = NULL;

  for (auto p: this->pages) {
    if (last_page == NULL || last_page->access_time > p.second->access_time) {
      last_page = p.second;
    }
  }

  if (last_page == NULL) {
    Log::Fatal("Unable to find page to deallocate");
  }

  this->pages.erase(last_page->page_id);
  free(last_page->page);
  free(last_page);
}

}

