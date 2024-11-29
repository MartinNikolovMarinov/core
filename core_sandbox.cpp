#include "tests/t-index.h"

#include <iomanip>
#include <sstream>
#include <iostream>
#include <bitset>
#include <cstdio>

#include "tests/common/ryu_reference_impl/ryu_parse.h"

// constexpr const char* digits = "01.126";
// constexpr addr_size digitsSize = core::cstrLen(digits);
// constexpr auto res = core::cstrToFloat<f32>(digits, digitsSize);

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
    core::initProgramCtx(assertHandler, nullptr);
    std::cout << std::fixed << std::setprecision(17);

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(17);

    // 0x3F800000; // One in float
    // 0x3FF0000000000000; // One in double
    u64 n = 0x3FF0000000000000;

    bool above10 = false;
    bool above100 = false;
    bool above1000 = false;
    bool above10000 = false;

    for (; n < core::limitMax<u64>(); n++) {
        char s[64];
        f64 v = core::bitCast<f64>(n);
        i32 sN = std::sprintf(s, "%.16f", v);

        if (!above10 && v >= 10) {
            above10 = true;
            ss << std::fixed << std::setprecision(16);
        }
        else if (!above100 && v >= 10) {
            above100 = true;
            ss << std::fixed << std::setprecision(15);
        }
        else if (!above1000 && v >= 10) {
            above1000 = true;
            ss << std::fixed << std::setprecision(14);
        }
        else if (!above10000 && v >= 10) {
            above10000 = true;
            ss << std::fixed << std::setprecision(13);
        }

        f64 ryuRes;
        auto status = ryu::s2d_n(s, sN, &ryuRes);
        Assert(status == ryu::Status::SUCCESS);

        f64 myRes = core::Unpack(core::cstrToFloat<f64>(s, sN));

        Assert(ryuRes == myRes, "Failed to meet criteria");

        std::cout << s << " OK ->" << std::bitset<sizeof(n)*17>(n) << std::endl;
    }

    return 0;
}
