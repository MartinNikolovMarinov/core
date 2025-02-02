#include "tests/t-index.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

void assertHandler(const char* failedExpr, const char* file, i32 line, const char* funcName, const char* errMsg) {
    constexpr u32 stackFramesToSkip = 2;
    constexpr addr_size stackTraceBufferSize = core::CORE_KILOBYTE * 8;
    char trace[stackTraceBufferSize] = {};
    addr_size traceLen = 0;
    bool ok = core::stacktrace(trace, stackTraceBufferSize, traceLen, 200, stackFramesToSkip);

    std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                << "[ASSERTION]:\n  [EXPR]: " << failedExpr
                << "\n  [FUNC]: " << funcName
                << "\n  [FILE]: " << file << ":" << line
                << "\n  [MSG]: " << (errMsg ? errMsg : "") // IMPORTANT: MSVC's std implementation will crash if errMsg is nullptr !
                << ANSI_RESET()
                << std::endl;
    std::cout << ANSI_BOLD_START() << "[TRACE]:\n" << trace << ANSI_RESET() << std::endl;

    if (!ok) {
        std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                  << "Failed to take full stacktrace. Consider resizing the stacktrace buffer size!"
                  << ANSI_RESET()
                  << std::endl;
    }

    throw std::runtime_error("Assertion failed!");
};

i32 main() {
    u64 beginUnixTs = core::getUnixTimestampNowMs();
    u64 beginMonotonicTs = core::getMonotonicNowNs();
    u64 beginTsc = core::getPerfCounter();

    core::threadingSleep(5300);

    u64 endUnixTs = core::getUnixTimestampNowMs();
    u64 endMonotonicTs = core::getMonotonicNowNs();
    u64 endTsc = core::getPerfCounter();

    u64 freq = core::getCPUFrequencyHz();

    u64 elapsedTsc = endTsc - beginTsc;
    u64 elapsedNs = u64(f64(core::CORE_SECOND) * (f64(elapsedTsc) / f64(freq)));
    char elapsedTscStr[256];
    core::testing::elapsedTimeToStr(elapsedTscStr, elapsedNs);

    u64 elapsedUnixMs = endUnixTs - beginUnixTs;
    char elapsedUnixStr[256];
    core::testing::elapsedTimeToStr(elapsedUnixStr, elapsedUnixMs * core::CORE_MILLISECOND);

    u64 elapsedMonotonicNs = endMonotonicTs - beginMonotonicTs;
    char elapsedMonotonicStr[256];
    core::testing::elapsedTimeToStr(elapsedMonotonicStr, elapsedMonotonicNs);

    logInfo("CPU Frequency: %lluHz (%.4fGHz)", freq, f64(freq) / 1000000000.0);
    logInfo("Elapsed Timestamp Counter: %llu", elapsedTsc);
    logInfo("Elapsed Time: %s", elapsedTscStr);
    logInfo("Elapsed in Unix Timestamp: %s", elapsedUnixStr);
    logInfo("Elapsed in Monotonic Timestamp: %s", elapsedMonotonicStr);

    return 0;
}
