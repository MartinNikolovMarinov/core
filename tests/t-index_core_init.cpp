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

constexpr addr_size THREAD_LOCAL_GLOBAL_BUMP_ALLOCATOR_BUFFER_SIZE = core::CORE_KILOBYTE * 8;
thread_local u8 tl_bumpGlobalBuffer[THREAD_LOCAL_GLOBAL_BUMP_ALLOCATOR_BUFFER_SIZE];
static auto g_bumpGlobalBufferAllocator = core::ThreadLocalBumpAllocator::create(tl_bumpGlobalBuffer, THREAD_LOCAL_GLOBAL_BUMP_ALLOCATOR_BUFFER_SIZE);

constexpr addr_size THREAD_LOCAL_ARENA_ALLOCATOR_REGION_SIZE = core::CORE_KILOBYTE * 8;
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

// template<>
// addr_size core::hash(const i32& key) {
//     addr_size h = addr_size(core::simpleHash_32(reinterpret_cast<const void*>(&key), sizeof(key)));
//     return h;
// }

// template<>
// bool core::eq(const i32& a, const i32& b) {
//     return a == b;
// }

// template<>
// addr_size core::hash(const core::StrView& key) {
//     addr_size h = addr_size(core::simpleHash_32(key.data(), key.len()));
//     return h;
// }

// template<>
// bool core::eq(const core::StrView& a, const core::StrView& b) {
//     return a.eq(b);
// }

i32 runAllTests() {
    using namespace core::testing;

    coreInit();
    defer { coreShutdown(); };

    std::cout << "\n" << "RUNNING TESTS" << "\n\n";

    i32 ret = 0;
    TestSuiteInfo sInfo;

    // RunTestSuite(runAlgorithmsTestsSuite);
    sInfo.name = FN_NAME_TO_CPTR(runArrTestsSuite);
    if (runTestSuite(sInfo, runArrTestsSuite) != 0) { ret = -1; }
    // RunTestSuite(runBitsTestsSuite);
    // RunTestSuite(runCmdParserTestsSuite);
    // RunTestSuite(runCptrConvTestsSuite);
    // RunTestSuite(runCptrTestsSuite);
    // RunTestSuite(runDeferTestsSuite);
    // RunTestSuite(runExpectedTestsSuite);
    // RunTestSuite(runHashMapTestsSuite);
    // RunTestSuite(runHashTestsSuite);
    // RunTestSuite(runIntrinsicsTestsSuite);
    // RunTestSuite(runIntsTestsSuite);
    // RunTestSuite(runMathTestsSuite);
    // RunTestSuite(runMatrixTestsSuite);
    // RunTestSuite(runMemTestsSuite);
    sInfo.name = FN_NAME_TO_CPTR(runRndTestsSuite);
    if (runTestSuite(sInfo, runRndTestsSuite) != 0) { ret = -1; }
    // RunTestSuite(runStaticArrTestsSuite);
    // RunTestSuite(runStrBuilderTestsSuite);
    // RunTestSuite(runTransformsTestsSuite);
    // RunTestSuite(runTupleTestsSuite);
    // RunTestSuite(runUniquePtrTestsSuite);
    // RunTestSuite(runUtfTestsSuite);
    // RunTestSuite(runVecTestsSuite);

    sInfo.name = FN_NAME_TO_CPTR(runBumpAllocatorTestsSuite);
    if (runTestSuite(sInfo, runBumpAllocatorTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runStdAllocatorTestsSuite);
    if (runTestSuite(sInfo, runStdAllocatorTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runStdStatsAllocatorTestsSuite);
    if (runTestSuite(sInfo, runStdStatsAllocatorTestsSuite) != 0) { ret = -1; }
    sInfo.name = FN_NAME_TO_CPTR(runArenaAllocatorTestsSuite);
    if (runTestSuite(sInfo, runArenaAllocatorTestsSuite) != 0) { ret = -1; }

    // // Run platform specific tests:

    // #if defined(CORE_DEBUG) && CORE_DEBUG == 1
    //     // Stacktrace should only be expected to work in debug builds.
    //     RunTestSuite(runPltStacktraceTestsSuite);
    // #endif
    // RunTestSuite(runPltThreadingTestsSuite);
    // RunTestSuite(runPltTimeTestsSuite);
    // RunTestSuite(runPltErrorTestsSuite);
    // RunTestSuite(runPltPagesTestsSuite);
    // RunTestSuite(runPltFileSystemTestsSuite);

    std::cout << '\n';
    if (ret == 0) {
        std::cout << ANSI_BOLD(ANSI_GREEN("Tests OK")) << std::endl;
    }
    else {
        std::cout << ANSI_BOLD(ANSI_RED("Tests FAILED")) << std::endl;
    }

    if constexpr (CORE_RUN_COMPILETIME_TESTS != 1) {
        std::cout << ANSI_YELLOW_START() << ANSI_BOLD_START()
                  << "[WARN] DID NOT RUN COMPILETIME TESTS!"
                  << ANSI_RESET() << std::endl;
    }

    return ret;
}
