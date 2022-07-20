#pragma once

#include "API.h"
#include "types.h"

namespace core
{

using namespace coretypes;

CORE_API_EXPORT void MemCopy(void* dest, const void* src, ptr_size len);
CORE_API_EXPORT void* MemSet(void* dest, i32 c, ptr_size n);

} // namespace core
