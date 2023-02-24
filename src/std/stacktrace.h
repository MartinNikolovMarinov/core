#pragma once

#include <API.h>
#include <types.h>

#include <string>

namespace core {

using namespace coretypes;

CORE_API_EXPORT std::string stacktrace(u32 nStackFrames, u32 skipFrames = 1);

} // namespace core
