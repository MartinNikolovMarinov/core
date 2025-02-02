#pragma once

#include <core_API.h>
#include <core_arr.h>
#include <core_logger.h>
#include <core_mem.h>
#include <core_types.h>

#include <plt/core_time.h>

namespace core {

using namespace coretypes;

struct ProfileTimePoint {
    u64 elapsedExclusiveTsc;
    u64 elapsedInclusiveTsc;
    u64 hitCount;
    const char* label;
    u64 processedBytes;
    constexpr bool isUsed() { return hitCount > 0; }
};

struct ProfileBlock {
    char const *label;
    u64 startTsc;
    u64 oldElapsedInclusiveTsc;
    u32 timepointIdx;
    u32 parentBlockIdx;
};

struct ProfileResult {
    core::Memory<ProfileTimePoint> timepoints;
    u64 cpuFrequencyHz;
    u64 totalElapsedTsc;
    u64 totalElapsedNs;
};

ProfileTimePoint& getProfileTimePoint(addr_size idx);
void beginProfile();
ProfileResult endProfile();
void logProfileResult(const ProfileResult& result, core::LogLevel logLevel);

void __setGlobalProfileBlock(u32 idx);
u32 __getGlobalProfileBlock();

#define TIME_BLOCK2(name, idx, size)                                                                               \
    /* Create a unique block variable for this time block */                                                       \
    core::ProfileBlock CORE_NAME_CONCAT(block, __LINE__);                                                          \
    CORE_NAME_CONCAT(block, __LINE__).label = name;                                                                \
    CORE_NAME_CONCAT(block, __LINE__).timepointIdx = idx;                                                          \
    CORE_NAME_CONCAT(block, __LINE__).startTsc = core::getPerfCounter();                                           \
    CORE_NAME_CONCAT(block, __LINE__).parentBlockIdx = core::__getGlobalProfileBlock();                            \
    CORE_NAME_CONCAT(block, __LINE__).oldElapsedInclusiveTsc = core::getProfileTimePoint(idx).elapsedInclusiveTsc; \
                                                                                                                   \
    /* Set this block as the current global block */                                                               \
    core::__setGlobalProfileBlock(idx);                                                                            \
                                                                                                                   \
    /* At scope exit, update timings for the current block and its parent */                                       \
    defer {                                                                                                        \
        u64 elapsedTsc = core::getPerfCounter() - CORE_NAME_CONCAT(block, __LINE__).startTsc;                      \
        core::__setGlobalProfileBlock(CORE_NAME_CONCAT(block, __LINE__).parentBlockIdx);                           \
        auto& parentBlock = core::getProfileTimePoint(CORE_NAME_CONCAT(block, __LINE__).parentBlockIdx);           \
        auto& currentBlock = core::getProfileTimePoint(idx);                                                       \
        parentBlock.elapsedExclusiveTsc -= elapsedTsc;                                                             \
        currentBlock.elapsedExclusiveTsc += elapsedTsc;                                                            \
        currentBlock.elapsedInclusiveTsc = CORE_NAME_CONCAT(block, __LINE__).oldElapsedInclusiveTsc + elapsedTsc;  \
        currentBlock.hitCount++;                                                                                   \
        currentBlock.label = name;                                                                                 \
        currentBlock.processedBytes = size;                                                                        \
    }
#define TIME_BLOCK(name) TIME_BLOCK2(name, __COUNTER__ + 1, 0)
#define THROUGHPUT_BLOCK(name, size) TIME_BLOCK2(name, __COUNTER__ + 1, size)
#define TIME_FUNCTION TIME_BLOCK(__func__)

} // namespace core
