//
// Created by Pawel Burzynski on 12/02/2017.
//

#ifndef APOLLO_STORAGE_UVCOMMON_H
#define APOLLO_STORAGE_UVCOMMON_H

#include <uv.h>

namespace apollo {

class UvCommon {
 public:
  static void OnAlloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
  static void OnHandleClose(uv_handle_t *handle);
};

}

#endif //APOLLO_STORAGE_UVCOMMON_H
