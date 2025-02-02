#include <core_mem.h>
#include <testing/testing_framework.h>

#include <cstdio>

namespace core::testing {

char* elapsedTimeToStr(char out[ELAPSED_TIME_TO_STR_BUFFER_SIZE], u64 deltaTimeNs) {
    constexpr u32 kBufferSize = MEMORY_USED_TO_STR_BUFFER_SIZE;

    core::memset(&out[0], char(0), kBufferSize); // Clear the output buffer

    // TODO2: [Implement a sane snprintf variant]
    //
    // Using "unsigned long long" instead of "u64", which is alias to uint64_t, because snprintf complains about the
    // type. On differnet Operating Systems "uint64_t" might be "unsigned long long" or "unsigned long" and these are
    // considered different by the static checker for snprintf and produce a false warning.

    if (deltaTimeNs >= core::CORE_HOUR) {
        u32 hours = static_cast<u32>(deltaTimeNs / core::CORE_HOUR);
        u32 minutes = static_cast<u32>((deltaTimeNs % core::CORE_HOUR) / core::CORE_MINUTE);
        u32 seconds = static_cast<u32>((deltaTimeNs % core::CORE_MINUTE) / core::CORE_SECOND);
        unsigned long long milliseconds = (deltaTimeNs % core::CORE_SECOND) / core::CORE_MILLISECOND;
        std::snprintf(&out[0], kBufferSize, "%02uh %02um %02us %llums", hours, minutes, seconds, milliseconds);
    }
    else if (deltaTimeNs >= core::CORE_MINUTE) {
        u32 minutes = static_cast<u32>(deltaTimeNs / core::CORE_MINUTE);
        u32 seconds = static_cast<u32>((deltaTimeNs % core::CORE_MINUTE) / core::CORE_SECOND);
        unsigned long long milliseconds = (deltaTimeNs % core::CORE_SECOND) / core::CORE_MILLISECOND;
        std::snprintf(&out[0], kBufferSize, "%02um %02us %llums", minutes, seconds, milliseconds);
    }
    else if (deltaTimeNs >= core::CORE_SECOND) {
        u32 seconds = static_cast<u32>(deltaTimeNs / core::CORE_SECOND);
        unsigned long long milliseconds = (deltaTimeNs % core::CORE_SECOND) / core::CORE_MILLISECOND;
        std::snprintf(&out[0], kBufferSize, "%02us %llums", seconds, milliseconds);
    }
    else if (deltaTimeNs >= core::CORE_MILLISECOND) {
        unsigned long long milliseconds = deltaTimeNs / core::CORE_MILLISECOND;
        std::snprintf(&out[0], kBufferSize, "%llums", milliseconds);
    }
    else {
        unsigned long long nanoseconds = deltaTimeNs;
        std::snprintf(&out[0], kBufferSize, "%lluns", nanoseconds);
    }

    return out;
}

char* memoryUsedToStr(char out[MEMORY_USED_TO_STR_BUFFER_SIZE], addr_size deltaMemory) {
    constexpr u32 kBufferSize = MEMORY_USED_TO_STR_BUFFER_SIZE;

    core::memset(reinterpret_cast<u8*>(&out[0]), u8(0), kBufferSize);

    if (deltaMemory >= 1024 * 1024 * 1024) {
        f64 gb = f64(deltaMemory) / (1024.0 * 1024.0 * 1024.0);
        snprintf(&out[0], kBufferSize, "%.2f GB", gb);
    }
    else if (deltaMemory >= 1024 * 1024) {
        f64 mb = f64(deltaMemory) / (1024.0 * 1024.0);
        snprintf(&out[0], kBufferSize, "%.2f MB", mb);
    }
    else if (deltaMemory >= 1024) {
        f64 kb = f64(deltaMemory) / 1024.0;
        snprintf(&out[0], kBufferSize, "%.2f KB", kb);
    }
    else {
        snprintf(&out[0], kBufferSize, "%lld B", static_cast<long long int>(deltaMemory));
    }

    return out;
}

} // namespace core::testing
