//
// Created by Pawel Burzynski on 17/01/2017.
//

#ifndef APOLLO_STORAGE_STORAGE_PAGE_H
#define APOLLO_STORAGE_STORAGE_PAGE_H

#include <cstdint>

typedef uint8_t volatile_t;


class StoragePage {
public:
    virtual ~StoragePage() {};
    virtual void Write(uint64_t offset, void *source, uint64_t size) = 0;
    virtual volatile_t *Read(uint64_t offset, uint64_t size) = 0;
    virtual uint64_t GetPageSize() = 0;
};

#endif //APOLLO_STORAGE_STORAGE_PAGE_H
