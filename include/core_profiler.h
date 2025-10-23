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
    u32 id;
    u32 parentId;
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

// The profiler is not thread-safe. It's recommended to create a separate profiler instance for each thread.
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
    u64 startTsc;
    u64 oldElapsedInclusiveTsc;
    u32 parentBlockIdx;
};

#define TIME_BLOCK2(name, profiler, idx, size)                                                                       \
    /* Create a unique block variable for this time block */                                                         \
    core::ProfileBlock CORE_NAME_CONCAT(block, __LINE__);                                                            \
    CORE_NAME_CONCAT(block, __LINE__).startTsc = core::getPerfCounter();                                             \
    CORE_NAME_CONCAT(block, __LINE__).parentBlockIdx = (profiler).globalBlockIdx;                                    \
    CORE_NAME_CONCAT(block, __LINE__).oldElapsedInclusiveTsc = (profiler).getTimePoint(idx).elapsedInclusiveTsc;     \
                                                                                                                     \
    /* Set this block as the current global block */                                                                 \
    (profiler).globalBlockIdx = idx;                                                                                 \
                                                                                                                     \
    /* At scope exit, update timings for the current block and its parent */                                         \
    defer {                                                                                                          \
        u64 elapsedTsc = core::getPerfCounter() - CORE_NAME_CONCAT(block, __LINE__).startTsc;                        \
        (profiler).globalBlockIdx = (CORE_NAME_CONCAT(block, __LINE__).parentBlockIdx);                              \
        auto& parentTimePoint = (profiler).getTimePoint(CORE_NAME_CONCAT(block, __LINE__).parentBlockIdx);           \
        auto& currTimePoint = (profiler).getTimePoint(idx);                                                          \
        parentTimePoint.elapsedExclusiveTsc -= elapsedTsc;                                                           \
        currTimePoint.elapsedExclusiveTsc += elapsedTsc;                                                             \
        currTimePoint.elapsedInclusiveTsc = CORE_NAME_CONCAT(block, __LINE__).oldElapsedInclusiveTsc + elapsedTsc;   \
        currTimePoint.hitCount++;                                                                                    \
        currTimePoint.label = name;                                                                                  \
        currTimePoint.processedBytes += size;                                                                        \
        currTimePoint.id = idx;                                                                                      \
        currTimePoint.parentId = (profiler).globalBlockIdx;                                                          \
    }


/**
 * @brief Creates a profiling block which measures elapsed time and/or data throughput inside a block scope.
 *
 * @param profilder - the profiler object which will keep all the necessary state.
 * @param timepointId - the UNIQUE identifier which is used to track timepoints in a caller hierarchy.
 *                      IMPORTANT: Behaviour is undefined if more than one time block uses the same identifier.
 *                      The Id of 0 is reserved, DO NOT USE IT.
 * @param label - the label which is associated with the time block MUST be a compiletime const char pointer.
 * @param size - the throughput that will pass through this block.
 *
 * @return An error code if the thread could not be started.
*/
#define TIME_BLOCK(profiler, timepointId, label) TIME_BLOCK2(label, profiler, timepointId, 0)
#define THROUGHPUT_BLOCK(profiler, timepointId, label, size) TIME_BLOCK2(label, profiler, timepointId, size)
#define TIME_FUNCTION(profiler, timepointId) TIME_BLOCK(profiler, timepointId, __func__)

} // namespace core
