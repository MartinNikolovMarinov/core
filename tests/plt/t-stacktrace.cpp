#include "../t-index.h"

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

i32 testFn2() {
    constexpr addr_size BUF_MAX = 4096;
    char buf[BUF_MAX] = {};
    addr_size bufWritten = 0;
    bool ok = core::stacktrace(buf, BUF_MAX, bufWritten, 4);

    Assert(ok);
    Assert(bufWritten > 0);
    Assert(bufWritten < BUF_MAX);
    Assert(buf[0] != '\0');
    Assert(core::cptrLen(buf) == bufWritten);

    std::string traceStr(buf, bufWritten);
    Assert(traceStr.find("stacktraceFunctionCheckTest") != std::string::npos);
    Assert(traceStr.find("testFn1") != std::string::npos);
    Assert(traceStr.find("testFn2") != std::string::npos);

    return 0;
}

i32 testFn1() { return testFn2(); }

i32 stacktraceFunctionCheckTest() { return testFn1(); }

inline i32 testFn3() {
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
    Assert(traceStr.find("stacktraceOnInlinedTest") != std::string::npos);

    return 0;
}

i32 stacktraceOnInlinedTest() {
    return testFn3();
}

i32 runPltStacktraceTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = {};
    tInfo.trackMemory = false;

    tInfo.name = FN_NAME_TO_CPTR(callingStacktraceDoesNotCrashTest);
    if (runTest(tInfo, callingStacktraceDoesNotCrashTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(stacktraceFunctionCheckTest);
    if (runTest(tInfo, stacktraceFunctionCheckTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(stacktraceOnInlinedTest);
    if (runTest(tInfo, stacktraceOnInlinedTest) != 0) { ret = -1; }

    return ret;
}
