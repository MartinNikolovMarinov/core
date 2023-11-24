#include "../index.h"

i32 callingStacktraceDoesNotCrashTest() {
    constexpr addr_size BUF_MAX = 4096;
    char buf[BUF_MAX] = {};
    addr_size bufWritten = 0;
    bool ok = core::stacktrace(buf, BUF_MAX, bufWritten, 20);

    Assert(ok);
    Assert(bufWritten > 0);
    Assert(bufWritten < BUF_MAX);
    Assert(buf[0] != '\0');
    Assert(core::cptrLen(buf) == bufWritten);

    return 0;
}

i32 stacktraceContainsThisFunctionTest() {
    constexpr addr_size BUF_MAX = 4096;
    char buf[BUF_MAX] = {};
    addr_size bufWritten = 0;
    bool ok = core::stacktrace(buf, BUF_MAX, bufWritten, 3);

    Assert(ok);
    Assert(bufWritten > 0);
    Assert(bufWritten < BUF_MAX);
    Assert(buf[0] != '\0');
    Assert(core::cptrLen(buf) == bufWritten);

    std::string traceStr(buf, bufWritten);
    Assert(traceStr.find("stacktraceContainsThisFunctionTest") != std::string::npos);
    Assert(traceStr.find("runPltStacktraceTestsSuite") != std::string::npos);

    return 0;
}

inline i32 stacktraceOnInlinedTest() {
    constexpr addr_size BUF_MAX = 4096;
    char buf[BUF_MAX] = {};
    addr_size bufWritten = 0;
    bool ok = core::stacktrace(buf, BUF_MAX, bufWritten, 3);

    Assert(ok);
    Assert(bufWritten > 0);
    Assert(bufWritten < BUF_MAX);
    Assert(buf[0] != '\0');
    Assert(core::cptrLen(buf) == bufWritten);

    std::string traceStr(buf, bufWritten);
    Assert(traceStr.find("runPltStacktraceTestsSuite") != std::string::npos);

    return 0;
}

i32 runPltStacktraceTestsSuite() {
    RunTest(callingStacktraceDoesNotCrashTest);
    RunTest(stacktraceContainsThisFunctionTest);
    RunTest(stacktraceOnInlinedTest);

    return 0;
}
