#pragma once

// #################### CORE LIBRARY CONFIGURATIONS ####################################################################

#define CORE_DEFAULT_ALLOCATOR() core::StdAllocator

#include <core.h>
#include <core.h> // If pragma once is omitted somewhere in the core library, this will catch it.

#include <iostream>

using namespace coretypes;

void coreInit();

template<> addr_size core::hash(const i32& key);
template<> bool core::eq(const i32& a, const i32& b);

template<> addr_size core::hash(const core::StrView& key);
template<> bool core::eq(const core::StrView& a, const core::StrView& b);

// #################### TESTING HELPERS ################################################################################

#if defined(CORE_RUN_COMPILETIME_TESTS) && CORE_RUN_COMPILETIME_TESTS == 1
    #define RunTestCompileTime(test, ...) \
        { [[maybe_unused]] constexpr auto __notused__ = core::force_consteval<test(__VA_ARGS__)>; }
#else
    #define RunTestCompileTime(...)
    #define CORE_RUN_COMPILETIME_TESTS 0
#endif

// ##################### Test suites ###################################################################################

i32 runAlgorithmsTestsSuite();
i32 runArrTestsSuite();
i32 runBitsTestsSuite();
i32 runCmdParserTestsSuite();
i32 runCptrConvTestsSuite();
i32 runCptrTestsSuite();
i32 runDeferTestsSuite();
i32 runExpectedTestsSuite();
i32 runHashMapTestsSuite();
i32 runHashTestsSuite();
i32 runIntrinsicsTestsSuite();
i32 runIntsTestsSuite();
i32 runMathTestsSuite();
i32 runMatrixTestsSuite();
i32 runMemTestsSuite();
i32 runRndTestsSuite();
i32 runStaticArrTestsSuite();
i32 runStrBuilderTestsSuite();
i32 runTraitsTestsSuite();
i32 runTransformsTestsSuite();
i32 runTupleTestsSuite();
i32 runUniquePtrTestsSuite();
i32 runUtfTestsSuite();
i32 runVecTestsSuite();

i32 runBumpAllocatorTestsSuite();
i32 runStdAllocatorTestsSuite();
i32 runStdStatsAllocatorTestsSuite();

i32 runPltErrorTestsSuite();
i32 runPltFileSystemTestsSuite();
i32 runPltPagesTestsSuite();
i32 runPltPathTestsSuite();
i32 runPltStacktraceTestsSuite();
i32 runPltThreadingTestsSuite();
i32 runPltTimeTestsSuite();

i32 runAllTests();
