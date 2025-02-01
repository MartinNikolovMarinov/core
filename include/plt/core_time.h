#pragma once

#include <core_API.h>
#include <core_types.h>

namespace core {

using namespace coretypes;

CORE_API_EXPORT u64 getUnixTimestampNowMs();
CORE_API_EXPORT u64 getMonotonicNowNs();

CORE_API_EXPORT u64 getPerfCounter();
CORE_API_EXPORT u64 getCPUFrequencyHz(); // estimated

} // namespace core
