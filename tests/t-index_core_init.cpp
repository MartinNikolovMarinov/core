#include "t-index.h"

namespace {

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

static core::StdStatsAllocator g_stdStatsAllocator;
static core::BumpAllocator g_bumpAllocator;
static core::StdArenaAllocator g_arenaAllocator(256);

constexpr addr_size THREAD_LOCAL_GLOBAL_BUMP_ALLOCATOR_BUFFER_SIZE = core::CORE_MEGABYTE;
thread_local u8 tl_bumpGlobalBuffer[THREAD_LOCAL_GLOBAL_BUMP_ALLOCATOR_BUFFER_SIZE];
static auto g_threadLocalBumpAllocator = core::ThreadLocalBumpAllocator::create(tl_bumpGlobalBuffer, THREAD_LOCAL_GLOBAL_BUMP_ALLOCATOR_BUFFER_SIZE);

constexpr addr_size THREAD_LOCAL_ARENA_ALLOCATOR_REGION_SIZE = core::CORE_MEGABYTE;
static auto g_threadLocalArenaAllocator = core::ThreadLocalStdArenaAllocator::create(THREAD_LOCAL_ARENA_ALLOCATOR_REGION_SIZE);

void coreInit() {
    core::initProgramCtx(assertHandler);

    core::registerAllocator(core::createAllocatorCtx(&g_stdStatsAllocator), RA_STD_STATS_ALLOCATOR_ID);
    core::registerAllocator(core::createAllocatorCtx(&g_bumpAllocator), RA_BUMP_ALLOCATOR_ID);
    core::registerAllocator(core::createAllocatorCtx(&g_arenaAllocator), RA_ARENA_ALLOCATOR_ID);
    core::registerAllocator(core::createAllocatorCtx(&g_threadLocalBumpAllocator), RA_THREAD_LOCAL_BUMP_ALLOCATOR_ID);
    core::registerAllocator(core::createAllocatorCtx(&g_threadLocalArenaAllocator), RA_THREAD_LOCAL_ARENA_ALLOCATOR_ID);
}

void coreShutdown() {
    core::destroyProgramCtx();
}

} // namespace

void setBufferForBumpAllocator(void* data, addr_size size) {
    g_bumpAllocator.setBuffer(data, size);
}

void setBlockSizeForArenaAllocator(addr_size size) {
    g_arenaAllocator.setBlockSize(size);
}

i32 runAllTests() {
    using namespace core::testing;

    coreInit();
    defer { coreShutdown(); };

    std::cout << "\n" << "RUNNING TESTS" << "\n\n";

    i32 ret = 0;
    TestSuiteInfo sInfo;
    sInfo.useAnsiColors = g_useAnsi;

    // sInfo.name = FN_NAME_TO_CPTR(runAlgorithmsTestsSuite);
    // if (runTestSuite(sInfo, runAlgorithmsTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runArrTestsSuite);
    // if (runTestSuite(sInfo, runArrTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runBitsTestsSuite);
    // if (runTestSuite(sInfo, runBitsTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runCmdParserTestsSuite);
    // if (runTestSuite(sInfo, runCmdParserTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runCstrConvTestsSuite);
    if (runTestSuite(sInfo, runCstrConvTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runCstrConv_CstrToFloat_TestsSuite);
    // if (runTestSuite(sInfo, runCstrConv_CstrToFloat_TestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runCstrConv_FloatToCstr_TestsSuite);
    // if (runTestSuite(sInfo, runCstrConv_FloatToCstr_TestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runCstrTestsSuite);
    // if (runTestSuite(sInfo, runCstrTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runDeferTestsSuite);
    // if (runTestSuite(sInfo, runDeferTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runExpectedTestsSuite);
    // if (runTestSuite(sInfo, runExpectedTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runHashMapTestsSuite);
    // if (runTestSuite(sInfo, runHashMapTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runHashTestsSuite);
    // if (runTestSuite(sInfo, runHashTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runIntrinsicsTestsSuite);
    // if (runTestSuite(sInfo, runIntrinsicsTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runUtfTestsSuite);
    // if (runTestSuite(sInfo, runUtfTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runIntsTestsSuite);
    // if (runTestSuite(sInfo, runIntsTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runMathTestsSuite);
    // if (runTestSuite(sInfo, runMathTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runVecTestsSuite);
    // if (runTestSuite(sInfo, runVecTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runMatrixTestsSuite);
    // if (runTestSuite(sInfo, runMatrixTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runTransformsTestsSuite);
    // if (runTestSuite(sInfo, runTransformsTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runMemTestsSuite);
    // if (runTestSuite(sInfo, runMemTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runRndTestsSuite);
    // if (runTestSuite(sInfo, runRndTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runStaticArrTestsSuite);
    // if (runTestSuite(sInfo, runStaticArrTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runStrBuilderTestsSuite);
    // if (runTestSuite(sInfo, runStrBuilderTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runTupleTestsSuite);
    // if (runTestSuite(sInfo, runTupleTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runUniquePtrTestsSuite);
    // if (runTestSuite(sInfo, runUniquePtrTestsSuite)) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runLoggerTestsSuite);
    // if (runTestSuite(sInfo, runLoggerTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runFormatTestsSuite);
    // if (runTestSuite(sInfo, runFormatTestsSuite) != 0) { ret = -1; }

    // sInfo.name = FN_NAME_TO_CPTR(runBumpAllocatorTestsSuite);
    // if (runTestSuite(sInfo, runBumpAllocatorTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runStdAllocatorTestsSuite);
    // if (runTestSuite(sInfo, runStdAllocatorTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runStdStatsAllocatorTestsSuite);
    // if (runTestSuite(sInfo, runStdStatsAllocatorTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runArenaAllocatorTestsSuite);
    // if (runTestSuite(sInfo, runArenaAllocatorTestsSuite) != 0) { ret = -1; }

    // // Run platform specific tests:

    // #if defined(CORE_DEBUG) && CORE_DEBUG == 1
    //     // Stacktrace should only be expected to work in debug builds.
    //     sInfo.name = FN_NAME_TO_CPTR(runPltStacktraceTestsSuite);
    //     if (runTestSuite(sInfo, runPltStacktraceTestsSuite) != 0) { ret = -1; }
    // #endif
    // sInfo.name = FN_NAME_TO_CPTR(runPltTimeTestsSuite);
    // if (runTestSuite(sInfo, runPltTimeTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runPltErrorTestsSuite);
    // if (runTestSuite(sInfo, runPltErrorTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runPltPagesTestsSuite);
    // if (runTestSuite(sInfo, runPltPagesTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runPltFileSystemTestsSuite);
    // if (runTestSuite(sInfo, runPltFileSystemTestsSuite) != 0) { ret = -1; }
    // sInfo.name = FN_NAME_TO_CPTR(runPltThreadingTestsSuite);
    // if (runTestSuite(sInfo, runPltThreadingTestsSuite) != 0) { ret = -1; }

    sInfo.name = FN_NAME_TO_CPTR(runAsmTestsSuite);
    if (runTestSuite(sInfo, runAsmTestsSuite) != 0) { ret = -1; }

    // sInfo.name = FN_NAME_TO_CPTR(verifyRyuAlgorithm);
    // if (runTestSuite(sInfo, verifyRyuAlgorithm) != 0) { ret = -1; }

    std::cout << '\n';
    if (ret == 0) {
        if (sInfo.useAnsiColors) {
            std::cout << ANSI_BOLD(ANSI_GREEN("Tests OK")) << std::endl;
        }
        else {
            std::cout << "Tests OK" << std::endl;
        }
    }
    else {
        if (sInfo.useAnsiColors) {
            std::cout << ANSI_BOLD(ANSI_RED("Tests FAILED")) << std::endl;
        }
        else {
            std::cout << "Tests FAILED" << std::endl;
        }
    }

    if constexpr (CORE_RUN_COMPILETIME_TESTS != 1) {
        if (sInfo.useAnsiColors) {
            std::cout << ANSI_YELLOW_START() << ANSI_BOLD_START();
        }
        std::cout << "[WARN] DID NOT RUN COMPILETIME TESTS!";
        if (sInfo.useAnsiColors) {
            std::cout << ANSI_RESET() << std::endl;
        }
    }

    return ret;
}
