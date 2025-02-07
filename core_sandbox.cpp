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
    core::initProgramCtx(assertHandler);

    char buffer[256] = {};

    i32 len = core::Unpack(
        core::format(buffer, CORE_C_ARRLEN(buffer),
        "{} {} {} Iskam da si kupq {} {}",
        i8(-10), u8(15), f32(4.3512f), "RTX 4090 ", "yo"_sv)
    );

    logInfo("%s[%d]", buffer, len);

    return 0;
}
