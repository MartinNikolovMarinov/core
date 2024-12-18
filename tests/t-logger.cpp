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

i32 runLoggerTestsSuite() {
    using namespace core::testing;

    i32 ret = 0;
    TestInfo tInfo = createTestInfo();

    tInfo.name = FN_NAME_TO_CPTR(testLoggerLevelsTest);
    if (runTest(tInfo, testLoggerLevelsTest) != 0) { ret = -1; }
    tInfo.name = FN_NAME_TO_CPTR(muteLoggerTest);
    if (runTest(tInfo, muteLoggerTest) != 0) { ret = -1; }

    return ret;
}
