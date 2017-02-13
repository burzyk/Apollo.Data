//
// Created by Pawel Burzynski on 12/02/2017.
//

#ifndef SHAKADB_STORAGE_UVCOMMON_H
#define SHAKADB_STORAGE_UVCOMMON_H

#include <uv.h>

namespace shakadb {

class UvCommon {
 public:
  static void OnAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
  static void OnHandleClose(uv_handle_t *handle);
};

}

#endif //SHAKADB_STORAGE_UVCOMMON_H
