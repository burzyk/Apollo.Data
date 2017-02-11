//
// Created by Pawel Burzynski on 11/02/2017.
//

#include "allocator.h"

namespace apollo {

RwLock Allocator::lock;
std::map<void *, std::string> Allocator::memory;

}