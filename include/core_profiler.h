#pragma once

#include <core_API.h>
#include <core_arr.h>
#include <core_logger.h>
#include <core_mem.h>
#include <core_types.h>

#include <plt/core_time.h>

namespace core {

using namespace coretypes;

struct CORE_API_EXPORT ProfileTimePoint {
    u64 elapsedExclusiveTsc;
    u64 elapsedInclusiveTsc;
    u64 hitCount;
    const char* label;
    u64 processedBytes;
    constexpr bool isUsed() { return hitCount > 0; }
};

struct CORE_API_EXPORT ProfileResult {
    core::Memory<ProfileTimePoint> timepoints;
    u64 cpuFrequencyHz;
    u64 totalElapsedTsc;
    u64 totalElapsedNs;

    void logResult(core::LogLevel logLevel);
};

// The profiler is not thread-safe. It's recomended to create a separate profiler instance for each thread.
struct CORE_API_EXPORT Profiler {
    static constexpr addr_size MAX_TIMEPOINTS_COUNT = 4096;

    core::ArrStatic<ProfileTimePoint, MAX_TIMEPOINTS_COUNT> timepoints;
    u64 start;
    u64 end;
    u32 currTimepointIdx;
    u32 globalBlockIdx;

    constexpr Profiler() : timepoints({}), start(0), end(0), currTimepointIdx(0), globalBlockIdx(0) {}

    void beginProfile();
    ProfileResult endProfile();
    ProfileTimePoint& getTimePoint(addr_size idx);
};

struct CORE_API_EXPORT ProfileBlock {
    char const *label;
    u64 startTsc;
    u64 oldElapsedInclusiveTsc;
    u32 timepointIdx;
    u32 parentBlockIdx;
};

#define TIME_BLOCK2(name, profiler, idx, size)                                                                     \
    /* Create a unique block variable for this time block */                                                       \
    core::ProfileBlock CORE_NAME_CONCAT(block, __LINE__);                                                          \
    CORE_NAME_CONCAT(block, __LINE__).label = name;                                                                \
    CORE_NAME_CONCAT(block, __LINE__).timepointIdx = idx;                                                          \
    CORE_NAME_CONCAT(block, __LINE__).startTsc = core::getPerfCounter();                                           \
    CORE_NAME_CONCAT(block, __LINE__).parentBlockIdx = profiler.globalBlockIdx;                                    \
    CORE_NAME_CONCAT(block, __LINE__).oldElapsedInclusiveTsc = profiler.getTimePoint(idx).elapsedInclusiveTsc;     \
                                                                                                                   \
    /* Set this block as the current global block */                                                               \
    profiler.globalBlockIdx = idx;                                                                                 \
                                                                                                                   \
    /* At scope exit, update timings for the current block and its parent */                                       \
    defer {                                                                                                        \
        u64 elapsedTsc = core::getPerfCounter() - CORE_NAME_CONCAT(block, __LINE__).startTsc;                      \
        profiler.globalBlockIdx = (CORE_NAME_CONCAT(block, __LINE__).parentBlockIdx);                              \
        auto& parentBlock = profiler.getTimePoint(CORE_NAME_CONCAT(block, __LINE__).parentBlockIdx);               \
        auto& currentBlock = profiler.getTimePoint(idx);                                                           \
        parentBlock.elapsedExclusiveTsc -= elapsedTsc;                                                             \
        currentBlock.elapsedExclusiveTsc += elapsedTsc;                                                            \
        currentBlock.elapsedInclusiveTsc = CORE_NAME_CONCAT(block, __LINE__).oldElapsedInclusiveTsc + elapsedTsc;  \
        currentBlock.hitCount++;                                                                                   \
        currentBlock.label = name;                                                                                 \
        currentBlock.processedBytes = size;                                                                        \
    }

#define TIME_BLOCK(profiler, name) TIME_BLOCK2(name, profiler, __COUNTER__ + 1, 0)
#define THROUGHPUT_BLOCK(profiler, name, size) TIME_BLOCK2(name, profiler, __COUNTER__ + 1, size)
#define TIME_FUNCTION(profiler) TIME_BLOCK(profiler, __func__)

} // namespace core
