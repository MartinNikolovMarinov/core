#include "t-index.h"

namespace {
core::AllocatorContext g_allocators[AllocatorId::SENTINEL];
} // namespace

core::AllocatorContext* getAllocatorCtx(AllocatorId id) {
    auto* allocator =  &g_allocators[id];
    return allocator;
}

namespace {

void asserHandler(const char* failedExpr, const char* file, i32 line, const char* funcName, const char* errMsg) {
    constexpr u32 stackFramesToSkip = 3;
    constexpr addr_size stackTraceBufferSize = core::CORE_KILOBYTE * 8;
    char trace[stackTraceBufferSize] = {};
    addr_size traceLen = 0;
    bool ok = core::stacktrace(trace, stackTraceBufferSize, traceLen, 200, stackFramesToSkip);

    std::cout << ANSI_RED_START() << ANSI_BOLD_START()
                << "[ASSERTION]:\n  [EXPR]: " << failedExpr
                << "\n  [FUNC]: " << funcName
                << "\n  [FILE]: " << file << ":" << line
                << "\n  [MSG]: " << errMsg
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

constexpr addr_size THREAD_LOCAL_GLOBAL_BUMP_ALLOCATOR_BUFFER_SIZE = core::CORE_MEGABYTE;
thread_local u8 tl_bumpGlobalBuffer[THREAD_LOCAL_GLOBAL_BUMP_ALLOCATOR_BUFFER_SIZE];
static auto g_bumpGlobalBufferAllocator = core::ThreadLocalBumpAllocator::create(tl_bumpGlobalBuffer, THREAD_LOCAL_GLOBAL_BUMP_ALLOCATOR_BUFFER_SIZE);

constexpr addr_size THREAD_LOCAL_ARENA_ALLOCATOR_REGION_SIZE = core::CORE_MEGABYTE;
static auto g_arenaAllocator = core::ThreadLocalStdArenaAllocator::create(THREAD_LOCAL_ARENA_ALLOCATOR_REGION_SIZE);

void coreInit() {
    core::initProgramCtx(asserHandler, nullptr);

    g_allocators[AllocatorId::STD_STATS_ALLOCATOR] = core::createAllocatorCtx(&g_stdStatsAllocator);
    g_allocators[AllocatorId::BUMP_ALLOCATOR] = core::createAllocatorCtx(&g_bumpGlobalBufferAllocator);
    g_allocators[AllocatorId::ARENA_ALLOCATOR] = core::createAllocatorCtx(&g_arenaAllocator);
}

void coreShutdown() {
    core::destroyProgramCtx();
}

} // namespace

i32 runAllTests() {
    using namespace core::testing;

    coreInit();
    defer { coreShutdown(); };

    std::cout << "\n" << "RUNNING TESTS" << "\n\n";

    i32 ret = 0;
    TestSuiteInfo sInfo;
    sInfo.useAnsiColors = g_useAnsi;

    sInfo.name = FN_NAME_TO_CPTR(runAlgorithmsTestsSuite);
    if (runTestSuite(sInfo, runAlgorithmsTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runArrTestsSuite);
    if (runTestSuite(sInfo, runArrTestsSuite) != 0) { ret = -1; }
    // RunTestSuite(runCmdParserTestsSuite); // FIXME:
    sInfo.name = FN_NAME_TO_CPTR(runBitsTestsSuite);
    if (runTestSuite(sInfo, runBitsTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runCptrConvTestsSuite);
    if (runTestSuite(sInfo, runCptrConvTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runCptrTestsSuite);
    if (runTestSuite(sInfo, runCptrTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runDeferTestsSuite);
    if (runTestSuite(sInfo, runDeferTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runExpectedTestsSuite);
    if (runTestSuite(sInfo, runExpectedTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runHashMapTestsSuite);
    if (runTestSuite(sInfo, runHashMapTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runHashTestsSuite);
    if (runTestSuite(sInfo, runHashTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runIntrinsicsTestsSuite);
    if (runTestSuite(sInfo, runIntrinsicsTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runUtfTestsSuite);
    if (runTestSuite(sInfo, runUtfTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runIntsTestsSuite);
    if (runTestSuite(sInfo, runIntsTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runMathTestsSuite);
    if (runTestSuite(sInfo, runMathTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runVecTestsSuite);
    if (runTestSuite(sInfo, runVecTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runMatrixTestsSuite);
    if (runTestSuite(sInfo, runMatrixTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runTransformsTestsSuite);
    if (runTestSuite(sInfo, runTransformsTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runMemTestsSuite);
    if (runTestSuite(sInfo, runMemTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runRndTestsSuite);
    if (runTestSuite(sInfo, runRndTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runStaticArrTestsSuite);
    if (runTestSuite(sInfo, runStaticArrTestsSuite) != 0) { ret = -1; }
    // RunTestSuite(runStrBuilderTestsSuite); // FIXME:
    sInfo.name = FN_NAME_TO_CPTR(runTupleTestsSuite);
    if (runTestSuite(sInfo, runTupleTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runUniquePtrTestsSuite);
    if (runTestSuite(sInfo, runUniquePtrTestsSuite)) { ret = -1; }

    sInfo.name = FN_NAME_TO_CPTR(runBumpAllocatorTestsSuite);
    if (runTestSuite(sInfo, runBumpAllocatorTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runStdAllocatorTestsSuite);
    if (runTestSuite(sInfo, runStdAllocatorTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runStdStatsAllocatorTestsSuite);
    if (runTestSuite(sInfo, runStdStatsAllocatorTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runArenaAllocatorTestsSuite);
    if (runTestSuite(sInfo, runArenaAllocatorTestsSuite) != 0) { ret = -1; }

    // Run platform specific tests:

    #if defined(CORE_DEBUG) && CORE_DEBUG == 1
        // Stacktrace should only be expected to work in debug builds.
        sInfo.name = FN_NAME_TO_CPTR(runPltStacktraceTestsSuite);
        if (runTestSuite(sInfo, runPltStacktraceTestsSuite) != 0) { ret = -1; }
    #endif
    sInfo.name = FN_NAME_TO_CPTR(runPltTimeTestsSuite);
    if (runTestSuite(sInfo, runPltTimeTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runPltErrorTestsSuite);
    if (runTestSuite(sInfo, runPltErrorTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runPltPagesTestsSuite);
    if (runTestSuite(sInfo, runPltPagesTestsSuite) != 0) { ret = -1; }
    // RunTestSuite(runPltThreadingTestsSuite); // FIXME:
    // RunTestSuite(runPltFileSystemTestsSuite); // FIXME:

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
