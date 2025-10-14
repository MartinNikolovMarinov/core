#include <core_profiler.h>

#include <core_arr.h>
#include <core_assert.h>
#include <core_extensions/hash_functions.h>
#include <core_hash_map.h>
#include <core_logger.h>
#include <core_mem.h>
#include <core_stack.h>
#include <core_types.h>

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
    Assert(idx < MAX_TIMEPOINTS_COUNT, "idx out of range");
    return timepoints[idx];
}

namespace {

inline void logElapsed(addr_size i, u64 totalElapsedTsc, u64 freq, const ProfileTimePoint& a, const char* nestPadding) {
    char buffer[255];
    auto& label = a.label;

    // Print Exclusive
    {
        u64 hits = a.hitCount;
        u64 tsc = a.elapsedExclusiveTsc;
        u64 elapsedNS = u64(core::CORE_SECOND * (f64(tsc) / f64(freq)));
        core::testing::elapsedTimeToStr(buffer, elapsedNS);
        f64 percent = 100.0 * (f64(tsc) / f64(totalElapsedTsc));

        core::logDirectStd("{}{}. {} - {:f.2}%\n", nestPadding, i, label, percent);
        core::logDirectStd("{}    - Id         : {}\n", nestPadding, a.id);
        core::logDirectStd("{}    - Parent Id  : {}\n", nestPadding, a.parentId);
        core::logDirectStd("{}    - Hits       : {}\n", nestPadding, hits);
        core::logDirectStd("{}    - Self Time  : {} ({}, {:f.2}%)\n", nestPadding, buffer, tsc, percent);
    }

    // Print Inclusive
    if (a.elapsedExclusiveTsc != a.elapsedInclusiveTsc) {
        u64 tsc = a.elapsedInclusiveTsc;
        u64 elapsedNS = u64(core::CORE_SECOND * (f64(tsc) / f64(freq)));
        core::testing::elapsedTimeToStr(buffer, elapsedNS);
        f64 percent = 100.0 * (f64(tsc) / f64(totalElapsedTsc));
        core::logDirectStd("{}    - Total Time : {} ({}, {:f.2}%)\n", nestPadding, buffer, tsc, percent);
    }

    // Print Throughput
    if (a.processedBytes > 0) {
        u64 tsc = a.elapsedExclusiveTsc;
        char memBuffer[core::testing::MEMORY_USED_TO_STR_BUFFER_SIZE];
        core::testing::memoryUsedToStr(memBuffer, u64(f64(a.processedBytes) / (f64(tsc) / f64(freq))));
        core::logDirectStd("{}    - Throughput : {}/s \n", nestPadding, memBuffer);
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

    // TODO: It's probably useful to sort these by percentage total time.

    core::HashMap<u32, core::ArrList<u32>> parentToChildren;
    core::ArrList<u32> rootLevelPoints;

    for (addr_size i = 0; i < timepoints.len(); i++) {
        auto& t = timepoints[i];
        if (t.isUsed()) {
            if (!parentToChildren.contains(t.id)) {
                parentToChildren.put(t.id, {});
            }

            if (t.parentId) {
                if (!parentToChildren.contains(t.parentId)) {
                    parentToChildren.put(t.parentId, {});
                }

                parentToChildren.get(t.parentId)->push(t.id);
            }
            else {
                rootLevelPoints.push(t.id);
            }
        }
    }

    struct LogStackEntry {
        u32 id;
        u32 level;
    };

    bool visited[Profiler::MAX_TIMEPOINTS_COUNT] = {};
    char paddingBuffer[Profiler::MAX_TIMEPOINTS_COUNT];
    core::memset(paddingBuffer, ' ', Profiler::MAX_TIMEPOINTS_COUNT);
    core::Stack<LogStackEntry> logStack;

    for (addr_size i = 0; i < rootLevelPoints.len(); i++) {
        auto& rootTimePoint = timepoints[rootLevelPoints[i]];
        u32 nestedLevel = 0;

        logStack.clear();
        logStack.push({ rootTimePoint.id, nestedLevel });

        visited[rootTimePoint.id] = true;

        while (!logStack.empty()) {
            LogStackEntry curr = logStack.pop();
            auto currChildren = parentToChildren.get(curr.id);
            Assert(currChildren, "[BUG] curr children should exist in the map");

            for (addr_size j = 0; j < currChildren->len(); j++) {
                u32 cId = (*currChildren)[j];
                if (!visited[cId]) {
                    logStack.push({ cId, curr.level + 1 });
                    visited[cId] = true;
                }
            }

            // Log the current:
            {
                auto& t = timepoints[curr.id];
                paddingBuffer[curr.level] = '\0';
                logElapsed(curr.id, totalElapsedTsc, cpuFrequencyHz, t, paddingBuffer);
                paddingBuffer[curr.level] = ' ';
            }
        }
    }
}

} // namespace core
