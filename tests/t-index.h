#pragma once

#include <core.h>
#include <core.h> // If pragma once is omitted somewhere in the core library, this will catch it.

#include <core_extensions/hash_functions.h>
#include <core_extensions/hash_functions.h>

#include <iostream>

using namespace coretypes;

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

#if defined(CORE_TESTS_USE_ANSI) && CORE_TESTS_USE_ANSI == 1
constexpr bool g_useAnsi = true;
#else
constexpr bool g_useAnsi = false;
#endif

constexpr inline core::testing::TestInfo createTestInfo() {
    core::testing::TestInfo tInfo = {};
    tInfo.trackTicks = true;
    tInfo.trackTime = true;
    tInfo.trackMemory = false;
    tInfo.detectLeaks = false;
    tInfo.useAnsiColors = g_useAnsi;
    return tInfo;
}

template <typename TCallback>
void runForAllGlobalAllocatorVariants(TCallback cb, i32& retCode) {
    using namespace core::testing;

    {
        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = false; // the default allocator can't track memory allocations.
        tInfo.detectLeaks = false; // the default allocator can't detect memory leaks.
        cb(tInfo, "Default Allocator", retCode);
    }
    {
        core::AllocatorContext* actx = getAllocatorCtx(AllocatorId::STD_STATS_ALLOCATOR);
        core::setActiveAllocatorForThread(actx);
        defer {
            actx->clear();
            core::setActiveAllocatorForThread(nullptr);
        };
        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        tInfo.detectLeaks = true;
        cb(tInfo, "Stats STD Allocator", retCode);
    }
    {
        core::AllocatorContext* actx = getAllocatorCtx(AllocatorId::BUMP_ALLOCATOR);
        core::setActiveAllocatorForThread(actx);
        defer {
            actx->clear();
            core::setActiveAllocatorForThread(nullptr);
        };
        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        cb(tInfo, "THREAD LOCAL BUMP Allocator", retCode);
    }
    {
        core::AllocatorContext* actx = getAllocatorCtx(AllocatorId::ARENA_ALLOCATOR);
        core::setActiveAllocatorForThread(actx);
        defer {
            actx->clear();
            core::setActiveAllocatorForThread(nullptr);
        };
        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        cb(tInfo, "THREAD LOCAL ARENA Allocator", retCode);
    }
}

// a bit of macro magic never hurt nobody!@

#define USE_STACK_BASED_BUMP_ALLOCATOR_FOR_BLOCK_SCOPE(buff, size)          \
    core::BumpAllocator __lba__((buff), (size));                            \
    core::AllocatorContext __lbaCtx__ = core::createAllocatorCtx(&__lba__); \
    core::setActiveAllocatorForThread(&__lbaCtx__);                         \
    defer {                                                                 \
        __lba__.clear();                                                    \
        core::setActiveAllocatorForThread(nullptr);                         \
    }

#define USE_CUSTOM_ARENA_ALLOCATOR_FOR_FOR_BLOCK_SCOPE(regionSize)        \
    core::StdArenaAllocator __la__(regionSize);                           \
    core::AllocatorContext __laCtx__ = core::createAllocatorCtx(&__la__); \
    core::setActiveAllocatorForThread(&__laCtx__);                        \
    defer {                                                               \
        __la__.clear();                                                   \
        core::setActiveAllocatorForThread(nullptr);                       \
    }

// ##################### TEST SUITES ###################################################################################

i32 runAlgorithmsTestsSuite();
i32 runArrTestsSuite();
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

i32 runAsmTestsSuite();

i32 runAllTests();
