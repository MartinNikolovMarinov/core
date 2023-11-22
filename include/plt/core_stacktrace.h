#pragma once

#include <core_API.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

CORE_API_EXPORT void stacktrace(char* buf, addr_size bufMax, addr_size& bufWritten,
                                i32 nStackFrames, i32 skipFrames = 1);

} // namespace core
