#pragma once

#include <core.h>
#include <core.h> // If pragma once is omitted somewhere in the core library, this will catch it.

#include <core_extensions/hash_functions.h>
#include <core_extensions/hash_functions.h>

using namespace coretypes;

// #################### TESTING HELPERS ################################################################################

#if defined(CORE_TESTS_USE_ANSI) && CORE_TESTS_USE_ANSI == 1
constexpr bool g_useAnsi = true;
#else
constexpr bool g_useAnsi = false;
#endif

enum RegisteredAllocators : core::AllocatorId {
    RA_STD_ALLOCATOR_ID,
    RA_STD_STATS_ALLOCATOR_ID,
    RA_BUMP_ALLOCATOR_ID,
    RA_ARENA_ALLOCATOR_ID,
    RA_THREAD_LOCAL_BUMP_ALLOCATOR_ID,
    RA_THREAD_LOCAL_ARENA_ALLOCATOR_ID,

    RA_SENTINEL
};

constexpr inline core::testing::TestInfo createTestInfo() {
    core::testing::TestInfo tInfo = {};
    tInfo.trackTime = true;
    tInfo.trackMemory = false;
    tInfo.detectLeaks = false;
    tInfo.useAnsiColors = g_useAnsi;
    return tInfo;
}

constexpr inline core::testing::TestInfo createTestInfoFor(RegisteredAllocators id) {
    using namespace core::testing;

    TestInfo tInfo = createTestInfo();

    switch (id) {
        case RA_STD_ALLOCATOR_ID:
            tInfo.trackMemory = false; // the default allocator can't track memory allocations.
            tInfo.detectLeaks = false; // the default allocator can't detect memory leaks.
            tInfo.description = "STD Allocator";
            tInfo.allocatorContext = &core::getAllocator(id);
            return tInfo;
        case RA_STD_STATS_ALLOCATOR_ID:
            tInfo.trackMemory = true;
            tInfo.detectLeaks = true;
            tInfo.description = "Stats STD Allocator";
            tInfo.allocatorContext = &core::getAllocator(id);
            return tInfo;
        case RA_BUMP_ALLOCATOR_ID:
            tInfo.trackMemory = true;
            tInfo.description = "BUMP Allocator";
            tInfo.allocatorContext = &core::getAllocator(id);
            return tInfo;
        case RA_ARENA_ALLOCATOR_ID:
            tInfo.trackMemory = true;
            tInfo.description = "ARENA Allocator";
            tInfo.allocatorContext = &core::getAllocator(id);
            return tInfo;
        case RA_THREAD_LOCAL_BUMP_ALLOCATOR_ID:
            tInfo.trackMemory = true;
            tInfo.description = "THREAD LOCAL BUMP Allocator";
            tInfo.allocatorContext = &core::getAllocator(id);
            return tInfo;
        case RA_THREAD_LOCAL_ARENA_ALLOCATOR_ID:
            tInfo.trackMemory = true;
            tInfo.description = "THREAD LOCAL ARENA Allocator";
            tInfo.allocatorContext = &core::getAllocator(id);
            return tInfo;

        case RA_SENTINEL: [[fallthrough]];
        default:
            break;
    }

    Assert(false, "Invalid allocator id");
    return tInfo;
}

void setBufferForBumpAllocator(void* data, addr_size size);
void setBlockSizeForArenaAllocator(addr_size size);

// ##################### TEST SUITES ###################################################################################

i32 runAlgorithmsTestsSuite();
i32 runArrTestsSuite();
i32 runStackTestsSuite();
i32 runBitsTestsSuite();
i32 runCmdParserTestsSuite();
i32 runCstrConvTestsSuite();
i32 runCstrConv_CstrToFloat_TestsSuite();
i32 runCstrConv_FloatToCstr_TestsSuite();
i32 verifyRyuAlgorithm();
i32 runCstrTestsSuite();
i32 runDeferTestsSuite();
i32 runExpectedTestsSuite();
i32 runHashMapTestsSuite();
i32 runHashTestsSuite();
i32 runIntrinsicsTestsSuite();
i32 runIntsTestsSuite();
i32 runLoggerTestsSuite();
i32 runMathTestsSuite();
i32 runMatrixTestsSuite();
i32 runMemTestsSuite();
i32 runRndTestsSuite();
i32 runStaticArrTestsSuite();
i32 runStrBuilderTestsSuite();
i32 runTransformsTestsSuite();
i32 runTupleTestsSuite();
i32 runUniquePtrTestsSuite();
i32 runUtfTestsSuite();
i32 runVecTestsSuite();
i32 runFormatTestsSuite();

i32 runBumpAllocatorTestsSuite();
i32 runStdAllocatorTestsSuite();
i32 runStdStatsAllocatorTestsSuite();
i32 runArenaAllocatorTestsSuite();

i32 runPltErrorTestsSuite();
i32 runPltFileSystemTestsSuite();
i32 runPltPagesTestsSuite();
i32 runPltStacktraceTestsSuite();
i32 runPltThreadingTestsSuite();
i32 runPltTimeTestsSuite();

i32 runAsmTestsSuite();

i32 runAllTests();
