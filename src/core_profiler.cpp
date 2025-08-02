#include "core_logger.h"
#include <core_profiler.h>

#include <testing/testing_framework.h>

namespace core {

namespace {

constexpr addr_size MAX_TIMEPOINTS_COUNT = 4096;

struct Profiler {
    core::ArrStatic<ProfileTimePoint, MAX_TIMEPOINTS_COUNT> timepoints;
    u64 start;
    u64 end;
};

Profiler g_profiler;
u32 g_currTimepointIdx;

} // namespace

ProfileTimePoint& getProfileTimePoint(addr_size idx) {
    Assert(idx <= MAX_TIMEPOINTS_COUNT, "idx out of range");
    return g_profiler.timepoints[idx];
}

void beginProfile() {
    g_profiler.start = core::getPerfCounter();
    g_profiler.timepoints.replaceWith(ProfileTimePoint{}, MAX_TIMEPOINTS_COUNT);
    g_currTimepointIdx = 0;
}

ProfileResult endProfile() {
    g_profiler.end = core::getPerfCounter();

    u64 totalElapsedTsc = g_profiler.end - g_profiler.start;
    u64 freq = core::getCPUFrequencyHz();
    Assert(freq != 0, "Estimated CPU frequency must not be 0");

    ProfileResult result;
    result.cpuFrequencyHz = freq;
    result.totalElapsedTsc = totalElapsedTsc;
    result.totalElapsedNs = u64(core::CORE_SECOND * (f64(totalElapsedTsc) / f64(freq)));
    result.timepoints = core::Memory<ProfileTimePoint> { g_profiler.timepoints.data(), g_profiler.timepoints.len() };

    g_currTimepointIdx = 0;

    return result;
}

namespace {

inline void logElapsed(addr_size i, u64 totalElapsedTsc, u64 freq, const ProfileTimePoint& a) {
    char buffer[255];
    auto& label = a.label;
    u64 hits = a.hitCount;

    core::logDirectStd("{}. {}\n", i, label);
    core::logDirectStd("    - Hits       : {}\n", hits);

    // Print Exclusive
    {
        u64 tsc = a.elapsedExclusiveTsc;
        u64 elapsedNS = u64(core::CORE_SECOND * (f64(tsc) / f64(freq)));
        core::testing::elapsedTimeToStr(buffer, elapsedNS);
        f64 percent = 100.0 * (f64(tsc) / f64(totalElapsedTsc));
        core::logDirectStd("    - Self Time  : {} ({}, {:f.2}%)\n", buffer, tsc, percent);
    }

    // Print Inclusive
    if (a.elapsedExclusiveTsc != a.elapsedInclusiveTsc) {
        u64 tsc = a.elapsedInclusiveTsc;
        u64 elapsedNS = u64(core::CORE_SECOND * (f64(tsc) / f64(freq)));
        core::testing::elapsedTimeToStr(buffer, elapsedNS);
        f64 percent = 100.0 * (f64(tsc) / f64(totalElapsedTsc));
        core::logDirectStd("    - Total Time : {} ({}, {:f.2})\n", buffer, tsc, percent);
    }

    // Print Throughput
    if (a.processedBytes > 0) {
        u64 tsc = a.elapsedExclusiveTsc;
        char memBuffer[core::testing::MEMORY_USED_TO_STR_BUFFER_SIZE];
        core::testing::memoryUsedToStr(memBuffer, u64(f64(a.processedBytes) / (f64(tsc) / f64(freq))));
        core::logDirectStd("    - Throughput : {}/s \n", memBuffer);
    }
};

} // namespace

void logProfileResult(const ProfileResult& result, core::LogLevel logLevel) {
    if (logLevel < core::loggerGetLevel()) {
        return;
    }

    u64 freq = result.cpuFrequencyHz;
    u64 totalElapsedNs = result.totalElapsedNs;
    u64 totalElapsedTsc = result.totalElapsedTsc;

    char totalElapsedStr[core::testing::ELAPSED_TIME_TO_STR_BUFFER_SIZE];
    core::testing::elapsedTimeToStr(totalElapsedStr, totalElapsedNs);

    core::logDirectStd("--- CPU Profile Summary ---\n");
    core::logDirectStd("CPU Frequency : {} Hz ({:f.4} GHz)\n", freq, f64(freq) / 1000000000.0);
    core::logDirectStd("Total         : {}, {}\n", totalElapsedStr, totalElapsedTsc);
    core::logDirectStd("\n");

    for (addr_size i = 0; i < result.timepoints.len(); i++) {
        auto& a = result.timepoints[i];
        if (a.isUsed()) {
            logElapsed(i, totalElapsedTsc, freq, a);
        }
    }
}

void __setGlobalProfileBlock(u32 idx) {
    g_currTimepointIdx = idx;
}

u32 __getGlobalProfileBlock() {
    return g_currTimepointIdx;
}

} // namespace core
