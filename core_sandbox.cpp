#include "tests/t-index.h"

#include <iomanip>
#include <sstream>
#include <iostream>
#include <bitset>
#include <cstdio>

#include "tests/common/ryu_reference_impl/ryu.h"
#include "tests/common/ryu_reference_impl/ryu_parse.h"

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

// constexpr double d64_max = std::numeric_limits<double>::max();
// constexpr double d64_lowest = std::numeric_limits<double>::lowest();
// constexpr double d64_denorm_min = std::numeric_limits<double>::denorm_min();
// constexpr double d64_min = std::numeric_limits<double>::min();

// constexpr f64 tt = core::bitCast<f64>(1ull);

i32 main() {
    core::initProgramCtx(assertHandler, nullptr);

    {
        char buff[64];
        core::memset(buff, 5, 64);
        i32 n = core::floatToCstr(5.2f, buff, 64);
        buff[n] = '\0';
        std::cout << "buff = " << buff << " n = " << n << std::endl;
    }

    {
        char buff[64];
        i32 n = ryu::f2s_buffered_n(5.2f, buff);
        buff[n] = '\0';
        std::cout << "buff = " << buff << " n = " << n << std::endl;
    }


    return 0;
}
