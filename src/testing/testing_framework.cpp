#include <core_mem.h>
#include <core_cstr_format.h>
#include <testing/testing_framework.h>

namespace core::testing {

char* elapsedTimeToStr(char out[ELAPSED_TIME_TO_STR_BUFFER_SIZE], u64 deltaTimeNs) {
    constexpr u32 kBufferSize = ELAPSED_TIME_TO_STR_BUFFER_SIZE;

    core::memset(out, char(0), kBufferSize); // Clear the output buffer

    if (deltaTimeNs >= core::CORE_HOUR) {
        u32 hours = u32(deltaTimeNs / core::CORE_HOUR);
        u32 minutes = u32((deltaTimeNs % core::CORE_HOUR) / core::CORE_MINUTE);
        u32 seconds = u32((deltaTimeNs % core::CORE_MINUTE) / core::CORE_SECOND);
        u64 milliseconds = (deltaTimeNs % core::CORE_SECOND) / core::CORE_MILLISECOND;
        Unpack(core::format(out, kBufferSize,
            "{02:}h {02:}m {02:}s {}ms",
            hours, minutes, seconds, milliseconds));
    }
    else if (deltaTimeNs >= core::CORE_MINUTE) {
        u32 minutes = u32(deltaTimeNs / core::CORE_MINUTE);
        u32 seconds = u32((deltaTimeNs % core::CORE_MINUTE) / core::CORE_SECOND);
        u64 milliseconds = (deltaTimeNs % core::CORE_SECOND) / core::CORE_MILLISECOND;
        Unpack(core::format(out, kBufferSize,
            "{02:}m {02:}s {}ms",
            minutes, seconds, milliseconds));
    }
    else if (deltaTimeNs >= core::CORE_SECOND) {
        u32 seconds = u32(deltaTimeNs / core::CORE_SECOND);
        u64 milliseconds = (deltaTimeNs % core::CORE_SECOND) / core::CORE_MILLISECOND;
        Unpack(core::format(out, kBufferSize,
            "{02:}s {}ms",
            seconds, milliseconds));
    }
    else if (deltaTimeNs >= core::CORE_MILLISECOND) {
        u64 milliseconds = deltaTimeNs / core::CORE_MILLISECOND;
        u64 microseconds = (deltaTimeNs % core::CORE_MILLISECOND) / core::CORE_MICROSECOND;
        u64 nanoseconds = deltaTimeNs % core::CORE_MICROSECOND;
        Unpack(core::format(out, kBufferSize,
            "{}ms {}us {}ns",
            milliseconds, microseconds, nanoseconds));
    }
    else {
        u64 nanoseconds = deltaTimeNs;
        Unpack(core::format(out, kBufferSize,
            "{}ns",
            nanoseconds));
    }

    return out;
}

char* memoryUsedToStr(char out[MEMORY_USED_TO_STR_BUFFER_SIZE], addr_size deltaMemory) {
    constexpr u32 kBufferSize = MEMORY_USED_TO_STR_BUFFER_SIZE;

    core::memset(reinterpret_cast<u8*>(&out[0]), u8(0), kBufferSize);

    if (deltaMemory >= 1024 * 1024 * 1024) {
        f64 gb = f64(deltaMemory) / (1024.0 * 1024.0 * 1024.0);
        Unpack(core::format(out, kBufferSize, "{:f.2} GB", gb));
    }
    else if (deltaMemory >= 1024 * 1024) {
        f64 mb = f64(deltaMemory) / (1024.0 * 1024.0);
        Unpack(core::format(out, kBufferSize, "{:f.2} MB", mb));
    }
    else if (deltaMemory >= 1024) {
        f64 kb = f64(deltaMemory) / 1024.0;
        Unpack(core::format(out, kBufferSize, "{:f.2} KB", kb));
    }
    else {
        Unpack(core::format(out, kBufferSize, "{:f.2} B", f64(deltaMemory)));
    }

    return out;
}

TestInfo createTestInfo(const TestSuiteInfo& sinfo) {
    TestInfo tInfo = {};
    tInfo.trackTime = true;
    tInfo.useAnsiColors = sinfo.useAnsiColors;
    tInfo.actx = sinfo.actx;
    tInfo.trackMemory = sinfo.actx->tracksMemory();
    tInfo.detectLeaks = sinfo.actx->canDetectLeaks();
    tInfo.allocatorName = sinfo.actx->name();
    tInfo.expectZeroAllocations = sinfo.expectZeroAllocations;
    return tInfo;
}

TestInfo createTestInfo(core::AllocatorId allocatorId, bool useAnsiColors, bool expectZeroAllocation) {
    auto& actx = core::getAllocator(allocatorId);

    TestInfo tInfo = {};
    tInfo.trackTime = true;
    tInfo.useAnsiColors = useAnsiColors;
    tInfo.actx = &actx;
    tInfo.trackMemory = actx.tracksMemory();
    tInfo.detectLeaks = actx.canDetectLeaks();
    tInfo.allocatorName = actx.name();
    tInfo.expectZeroAllocations = expectZeroAllocation;
    return tInfo;
}

} // namespace core::testing
