#pragma once

#include <core_API.h>
#include <core_expected.h>
#include <core_types.h>
#include <plt/core_plt_error.h>

namespace core {

using namespace coretypes;

CORE_API_EXPORT expected<void*, PltErrCode> allocPages(size_t count);
CORE_API_EXPORT expected<PltErrCode> freePages(void* addr, size_t count);

} // namespace core
