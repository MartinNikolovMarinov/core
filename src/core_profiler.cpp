#include "core_logger.h"
#include <core_profiler.h>

#include <testing/testing_framework.h>

namespace core {

void Profiler::beginProfile() {
    start = core::getPerfCounter();
    timepoints.replaceWith(ProfileTimePoint{}, MAX_TIMEPOINTS_COUNT); // Clear with zero out
    currTimepointIdx = 0;
}

ProfileResult Profiler::endProfile() {
    end = core::getPerfCounter();

    u64 totalElapsedTsc = end - start;
    u64 freq = core::getCPUFrequencyHz();
    Assert(freq != 0, "Estimated CPU frequency must not be 0");

    ProfileResult result;
    result.cpuFrequencyHz = freq;
    result.totalElapsedTsc = totalElapsedTsc;
    result.totalElapsedNs = u64(core::CORE_SECOND * (f64(totalElapsedTsc) / f64(freq)));
    result.timepoints = core::Memory<ProfileTimePoint> { timepoints.data(), timepoints.len() };

    currTimepointIdx = 0;

    return result;
}

ProfileTimePoint& Profiler::getTimePoint(addr_size idx) {
    Assert(idx <= MAX_TIMEPOINTS_COUNT, "idx out of range");
    return timepoints[idx];
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

void ProfileResult::logResult(core::LogLevel logLevel) {
    if (logLevel < core::loggerGetLevel()) {
        return;
    }

    char totalElapsedStr[core::testing::ELAPSED_TIME_TO_STR_BUFFER_SIZE];
    core::testing::elapsedTimeToStr(totalElapsedStr, totalElapsedNs);

    core::logDirectStd("--- CPU Profile Summary ---\n");
    core::logDirectStd("CPU Frequency : {} Hz ({:f.4} GHz)\n", cpuFrequencyHz, f64(cpuFrequencyHz) / 1000000000.0);
    core::logDirectStd("Total         : {}, {}\n", totalElapsedStr, totalElapsedTsc);
    core::logDirectStd("\n");

    for (addr_size i = 0; i < timepoints.len(); i++) {
        auto& a = timepoints[i];
        if (a.isUsed()) {
            logElapsed(i, totalElapsedTsc, cpuFrequencyHz, a);
        }
    }
}

} // namespace core
