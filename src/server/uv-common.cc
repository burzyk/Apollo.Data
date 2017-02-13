//
// Created by Pawel Burzynski on 12/02/2017.
//

#include <uv.h>
#include <src/utils/allocator.h>
#include "uv-common.h"

namespace shakadb {

void UvCommon::OnAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
  buf->base = Allocator::New<char>(suggested_size);
  buf->len = suggested_size;
}

void UvCommon::OnHandleClose(uv_handle_t *handle) {
  Allocator::Delete(handle);
}

}