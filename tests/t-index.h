#pragma once

// #################### CORE LIBRARY CONFIGURATIONS ####################################################################

#define CORE_DEFAULT_ALLOCATOR() core::StdAllocator

#include <core.h>
#include <core.h> // If pragma once is omitted somewhere in the core library, this will catch it.

#include <iostream>

using namespace coretypes;

// template<> addr_size core::hash(const i32& key);
// template<> bool core::eq(const i32& a, const i32& b);

// template<> addr_size core::hash(const core::StrView& key);
// template<> bool core::eq(const core::StrView& a, const core::StrView& b);

// #################### TESTING HELPERS ################################################################################

enum AllocatorId : i32 {
    STD_STATS_ALLOCATOR,
    BUMP_ALLOCATOR,
    ARENA_ALLOCATOR,

    SENTINEL
};

core::AllocatorContext* getAllocatorCtx(AllocatorId id);

template <typename TAllocatorPtr>
inline TAllocatorPtr gatAllocatorByType(void* allocatorData) {
    static_assert(std::is_pointer_v<TAllocatorPtr>, "TAllocatorPtr must be a pointer type");
    return reinterpret_cast<TAllocatorPtr>(allocatorData);
}

template <typename TCallback>
void runForAllGlobalAllocatorVariants(TCallback cb, i32& retCode) {
    using namespace core::testing;

    {
        TestInfo tInfo = {};
        tInfo.trackMemory = false; // the default allocator can't track memory allocations.
        tInfo.detectLeaks = false; // the default allocator can't detect memory leaks.
        cb(tInfo, "Default Allocator", retCode);
    }
    {
        core::AllocatorContext* actx = getAllocatorCtx(AllocatorId::STD_STATS_ALLOCATOR);
        core::setActiveAllocatorForThread(actx);
        defer {
            actx->clear(actx->allocatorData);
            core::setActiveAllocatorForThread(nullptr);
        };
        TestInfo tInfo = {};
        tInfo.trackMemory = true;
        tInfo.detectLeaks = true;
        cb(tInfo, "Stats STD Allocator", retCode);
    }
    {
        core::AllocatorContext* actx = getAllocatorCtx(AllocatorId::BUMP_ALLOCATOR);
        core::setActiveAllocatorForThread(actx);
        defer {
            actx->clear(actx->allocatorData);
            core::setActiveAllocatorForThread(nullptr);
        };
        TestInfo tInfo = {};
        tInfo.trackMemory = true;
        cb(tInfo, "THREAD LOCAL BUMP Allocator", retCode);
    }
    {
        core::AllocatorContext* actx = getAllocatorCtx(AllocatorId::ARENA_ALLOCATOR);
        core::setActiveAllocatorForThread(actx);
        defer {
            actx->clear(actx->allocatorData);
            core::setActiveAllocatorForThread(nullptr);
        };
        TestInfo tInfo = {};
        tInfo.trackMemory = true;
        cb(tInfo, "THREAD LOCAL ARENA Allocator", retCode);
    }
}

#if defined(CORE_BUILD_TESTS_USE_ANSI) && CORE_BUILD_TESTS_USE_ANSI == 1
constexpr bool g_useAnsi = true;
#else
constexpr bool g_useAnsi = false;
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
i32 runTransformsTestsSuite();
i32 runTupleTestsSuite();
i32 runUniquePtrTestsSuite();
i32 runUtfTestsSuite();
i32 runVecTestsSuite();

i32 runBumpAllocatorTestsSuite();
i32 runStdAllocatorTestsSuite();
i32 runStdStatsAllocatorTestsSuite();
i32 runArenaAllocatorTestsSuite();

i32 runPltErrorTestsSuite();
i32 runPltFileSystemTestsSuite();
i32 runPltPagesTestsSuite();
i32 runPltPathTestsSuite();
i32 runPltStacktraceTestsSuite();
i32 runPltThreadingTestsSuite();
i32 runPltTimeTestsSuite();

i32 runAllTests();
