#include "t-index.h"

#include <stdio.h>
#include <stdarg.h>

void devNullLogHandler(const char*, ...) {}

i32 testLoggerLevelsTest() {
    defer { core::setLoggerLevel(core::LogLevel::L_TRACE); };

    struct TestCase {
        core::LogLevel level;
    };

    TestCase cases[] = {
        { core::LogLevel::L_TRACE },
        { core::LogLevel::L_DEBUG },
        { core::LogLevel::L_INFO },
        { core::LogLevel::L_WARNING },
        { core::LogLevel::L_ERROR },
        { core::LogLevel::L_FATAL },
    };

    i32 ret = core::testing::executeTestTable("test case failed at index: ", cases, [](auto& c, const char* cErr) {
        core::LogLevel level = c.level;
        core::LoggerCreateInfo createInfo = {};
        createInfo.print = devNullLogHandler;
        core::setLoggerLevel(level);

        CT_CHECK(core::initLogger(createInfo) == true, cErr);

        {
            bool res = logTrace("test");
            CT_CHECK(res == (level <= core::LogLevel::L_TRACE), cErr);
            res = logSectionTitleTraceTagged(0, "test");
            CT_CHECK(res == (level <= core::LogLevel::L_TRACE), cErr);
        }
        {
            bool res = logDebug("test");
            CT_CHECK(res == (level <= core::LogLevel::L_DEBUG), cErr);
            res = logSectionTitleDebugTagged(0, "test");
            CT_CHECK(res == (level <= core::LogLevel::L_DEBUG), cErr);
        }
        {
            bool res = logInfo("test");
            CT_CHECK(res == (level <= core::LogLevel::L_INFO), cErr);
            res = logSectionTitleInfoTagged(0, "test");
            CT_CHECK(res == (level <= core::LogLevel::L_INFO), cErr);
        }
        {
            bool res = logWarn("test");
            CT_CHECK(res == (level <= core::LogLevel::L_WARNING), cErr);
            res = logSectionTitleWarnTagged(0, "test");
            CT_CHECK(res == (level <= core::LogLevel::L_WARNING), cErr);
        }
        {
            bool res = logErr("test");
            CT_CHECK(res == (level <= core::LogLevel::L_ERROR), cErr);
            res = logSectionTitleErrTagged(0, "test");
            CT_CHECK(res == (level <= core::LogLevel::L_ERROR), cErr);
        }
        {
            bool res = logFatal("test");
            CT_CHECK(res == (level <= core::LogLevel::L_FATAL), cErr);
            res = logSectionTitleFatalTagged(0, "test");
            CT_CHECK(res == (level <= core::LogLevel::L_FATAL), cErr);
        }

        return 0;
    });

    return ret;
}

i32 muteLoggerTest() {
    core::setLoggerLevel(core::LogLevel::L_TRACE);
    defer { core::setLoggerLevel(core::LogLevel::L_TRACE); };

    CT_CHECK(logTrace(""));
    CT_CHECK(logDebug(""));
    CT_CHECK(logInfo(""));
    CT_CHECK(logWarn(""));
    CT_CHECK(logErr(""));
    CT_CHECK(logFatal(""));

    core::muteLogger(true);
    defer { core::muteLogger(false); };

    CT_CHECK(!logTrace(""));
    CT_CHECK(!logDebug(""));
    CT_CHECK(!logInfo(""));
    CT_CHECK(!logWarn(""));
    CT_CHECK(!logErr(""));
    CT_CHECK(!logFatal(""));

    return 0;
}

constexpr addr_size BUFFER_SIZE = core::CORE_KILOBYTE * 5;
static char buffer[BUFFER_SIZE] = {};
addr_size bufferWidx = 0;

void testPrintFn(const char* format, ...) {
    va_list args;
    va_start(args, format);
    i32 n = vsnprintf(buffer + bufferWidx, BUFFER_SIZE, format, args);
    bufferWidx += addr_size(n);
    va_end(args);
    Panic(bufferWidx < BUFFER_SIZE);
};

void clearBuffer() {
    core::memset(buffer, '\0', BUFFER_SIZE);
    bufferWidx = 0;
}

i32 verifyPrintingFnTest() {
    using LoggerCreateInfo = core::LoggerCreateInfo;

    i32 tagIndicesToIgnore[] = { 1, 4 };
    LoggerCreateInfo info;
    info.tagIndicesToIgnore = core::Memory(tagIndicesToIgnore, 2);
    info.print = testPrintFn;
    info.useAnsi = true;

    CT_CHECK(core::initLogger(info) == true);

    CT_CHECK(logTrace("t"));
    CT_CHECK(logDebug("d"));
    CT_CHECK(logInfo("i"));
    CT_CHECK(logWarn("w"));
    CT_CHECK(logErr("e"));
    CT_CHECK(logFatal("f"));
    CT_CHECK(core::logDirectStd("direct"));

    // std::cout << buffer << std::endl;
    clearBuffer();

    core::addLoggerTag("example"_sv);

    CT_CHECK(logTrace("t"));
    CT_CHECK(logDebug("d"));
    CT_CHECK(logInfo("i"));
    CT_CHECK(logWarn("w"));
    CT_CHECK(logErr("e"));
    CT_CHECK(logFatal("f"));
    CT_CHECK(core::logDirectStd("direct"));

    // std::cout << buffer << std::endl;
    clearBuffer();

    core::addLoggerTag("tag2"_sv);

    CT_CHECK(logTraceTagged(0, "t"));
    CT_CHECK(logDebugTagged(0, "d"));
    CT_CHECK(logInfoTagged(0, "i"));
    CT_CHECK(logWarnTagged(0, "w"));
    CT_CHECK(logErrTagged(0, "e"));
    CT_CHECK(logFatalTagged(0, "f"));
    CT_CHECK(core::logDirectStd("direct"));

    CT_CHECK(!logTraceTagged(1, "t"));
    CT_CHECK(!logDebugTagged(1, "d"));
    CT_CHECK(!logInfoTagged(1, "i"));
    CT_CHECK(!logWarnTagged(1, "w"));
    CT_CHECK(!logErrTagged(1, "e"));
    CT_CHECK(!logFatalTagged(1, "f"));
    CT_CHECK(core::logDirectStd(" direct2"));

    // std::cout << buffer << std::endl;
    clearBuffer();

    CT_CHECK(logSectionTitleTraceTagged(0, "t"));
    CT_CHECK(logSectionTitleDebugTagged(0, "d"));
    CT_CHECK(logSectionTitleInfoTagged(0, "i"));
    CT_CHECK(logSectionTitleWarnTagged(0, "w"));
    CT_CHECK(logSectionTitleErrTagged(0, "e"));
    CT_CHECK(logSectionTitleFatalTagged(0, "f"));
    CT_CHECK(core::logDirectStd("direct"));

    CT_CHECK(!logSectionTitleTraceTagged(1, "t"));
    CT_CHECK(!logSectionTitleDebugTagged(1, "d"));
    CT_CHECK(!logSectionTitleInfoTagged(1, "i"));
    CT_CHECK(!logSectionTitleWarnTagged(1, "w"));
    CT_CHECK(!logSectionTitleErrTagged(1, "e"));
    CT_CHECK(!logSectionTitleFatalTagged(1, "f"));
    CT_CHECK(core::logDirectStd(" direct2"));

    // std::cout << buffer << std::endl;
    clearBuffer();

    core::useAnsiInLogger(false);

    CT_CHECK(logSectionTitleTraceTagged(0, "t"));
    CT_CHECK(logSectionTitleDebugTagged(0, "d"));
    CT_CHECK(logSectionTitleInfoTagged(0, "i"));
    CT_CHECK(logSectionTitleWarnTagged(0, "w"));
    CT_CHECK(logSectionTitleErrTagged(0, "e"));
    CT_CHECK(logSectionTitleFatalTagged(0, "f"));
    CT_CHECK(core::logDirectStd("direct"));

    // std::cout << buffer << std::endl;
    clearBuffer();

    return 0;
}

i32 runLoggerTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(testLoggerLevelsTest);
    if (runTest(tInfo, testLoggerLevelsTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(muteLoggerTest);
    if (runTest(tInfo, muteLoggerTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(verifyPrintingFnTest);
    if (runTest(tInfo, verifyPrintingFnTest) != 0) { ret = -1; }

    return ret;
}
