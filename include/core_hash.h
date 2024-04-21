#pragma once

#include "core_API.h"
#include "core_types.h"

namespace core {

using namespace coretypes;

CORE_API_EXPORT u64 fnv1a_64(const void* input, addr_size len, u64 seed = 0);

CORE_API_EXPORT u64 djb2_64(const void* input, addr_size len, u64 seed = 0);

} // namespace core
