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
    UNDEFINED,

    RA_STD_ALLOCATOR_ID,
    RA_STD_STATS_ALLOCATOR_ID,
    RA_BUMP_ALLOCATOR_ID,
    RA_ARENA_ALLOCATOR_ID,
    RA_THREAD_LOCAL_BUMP_ALLOCATOR_ID,
    RA_THREAD_LOCAL_ARENA_ALLOCATOR_ID,

    RA_SENTINEL
};

void setBufferForBumpAllocator(void* data, addr_size size);
void setBlockSizeForArenaAllocator(addr_size size);

// ##################### TEST SUITES ###################################################################################

i32 runAlgorithmsTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runArrTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runStackTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runBitsTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runCmdParserTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runCstrConvTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runCstrConv_CstrToFloat_TestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runCstrConv_FloatToCstr_TestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 verifyRyuAlgorithm(const core::testing::TestSuiteInfo& sInfo);
i32 runCstrTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runDeferTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runExpectedTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runHashMapTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runHashTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runIntrinsicsTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runIntsTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runLoggerTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runMathTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runMatrixTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runMemTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runRndTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runStaticArrTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runStrBuilderTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runStrViewTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runTransformsTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runTupleTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runUniquePtrTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runUtfTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runVecTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runFormatTestsSuite(const core::testing::TestSuiteInfo& sInfo);

i32 runBumpAllocatorTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runStdAllocatorTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runStdStatsAllocatorTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runArenaAllocatorTestsSuite(const core::testing::TestSuiteInfo& sInfo);

i32 runPltErrorTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runPltFileSystemTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runPltPathTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runPltPagesTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runPltStacktraceTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runPltThreadingTestsSuite(const core::testing::TestSuiteInfo& sInfo);
i32 runPltTimeTestsSuite(const core::testing::TestSuiteInfo& sInfo);

i32 runAsmTestsSuite(const core::testing::TestSuiteInfo& sInfo);

i32 runAllTests();
