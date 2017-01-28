//
// Created by Pawel Burzynski on 21/01/2017.
//

#ifndef APOLLO_STORAGE_PAGE_ALLOCATOR_H
#define APOLLO_STORAGE_PAGE_ALLOCATOR_H

#include <map>
#include <list>
namespace apollo {

typedef int page_id_t;

struct page_info_t {
  uint8_t *page;
  page_id_t page_id;
  clock_t access_time;
};

class MemoryPageAllocator {
 public:
  MemoryPageAllocator(size_t page_size, int max_pages);
  ~MemoryPageAllocator();

  uint8_t *GetPage(page_id_t page_id);
  page_id_t AllocatePage();
 private:
  void DeallocateLastPage();

  size_t page_size;
  int max_pages;
  int next_page_id;

  std::map<int, page_info_t *> pages;
};

}

#endif //APOLLO_STORAGE_PAGEALLOCATOR_H
