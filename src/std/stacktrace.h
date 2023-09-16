#pragma once

#include <API.h>
#include <types.h>

#include <string>

namespace core {

using namespace coretypes;

CORE_API_EXPORT std::string stacktrace(i32 nStackFrames, i32 skipFrames = 1);

} // namespace core
