#include "t-index.h"

i32 initializeUniquePtrTest() {
    using namespace core::testing;

    {
        core::UniquePtr<i32> p;
        p = core::makeUnique<i32>(42);
        CT_CHECK(p.get() != nullptr);
        CT_CHECK(*p.get() == 42);
        CT_CHECK(*p == 42);
    }
    {
        core::UniquePtr<i32> p(core::makeUnique<i32>(42));
        CT_CHECK(p.get() != nullptr);
        CT_CHECK(*p.get() == 42);
        CT_CHECK(*p == 42);
    }
    {
        core::UniquePtr<i32> p = core::makeUnique<i32>(42);
        CT_CHECK(p.get() != nullptr);
        CT_CHECK(*p.get() == 42);
        CT_CHECK(*p == 42);
    }
    {
        auto p1 = core::makeUnique<i32>(42);
        auto p2 = core::makeUnique<i32>(24);
        CT_CHECK(p1.get() != nullptr);
        CT_CHECK(p2.get() != nullptr);
        CT_CHECK(*p1.get() == 42);
        CT_CHECK(*p2.get() == 24);
        CT_CHECK(*p1 == 42);
        CT_CHECK(*p2 == 24);

        p1 = std::move(p2);
        CT_CHECK(p1.get() != nullptr);
        CT_CHECK(p2.get() == nullptr);
        CT_CHECK(*p1.get() == 24);
        CT_CHECK(*p1 == 24);
    }
    {
        core::expected<core::UniquePtr<i32>, i32> p = core::makeUnique<i32>(42);
        CT_CHECK(p.hasValue());
        CT_CHECK(p.value().get() != nullptr);
        CT_CHECK(*p.value().get() == 42);
    }
    {
        defer { CT::resetAll(); };

        {
            core::UniquePtr<CT> p;
            CT_CHECK(CT::totalCtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
        }

        CT::resetAll();
        {
            auto p = core::makeUnique<CT>();
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
        }
        CT_CHECK(CT::dtorsCalled() == 1);

        CT::resetAll();
        {
            core::UniquePtr<CT> p;
            CT_CHECK(CT::totalCtorsCalled() == 0);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
            p = core::makeUnique<CT>();
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::dtorsCalled() == 0);
        }
        CT_CHECK(CT::dtorsCalled() == 1);

        CT::resetAll();
        {
            core::UniquePtr<CT> p;
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::totalCtorsCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
            p = core::makeUnique<CT>();
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::dtorsCalled() == 0);
            p = core::makeUnique<CT>();
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::totalCtorsCalled() == 2);
            CT_CHECK(CT::dtorsCalled() == 1);
        }
        CT_CHECK(CT::dtorsCalled() == 2);

        CT::resetAll();
        {
            CT* ct = core::construct<CT>();
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);

            core::UniquePtr<CT> p(ct);
            // Do not call the type's constructor when creating a UniquePtr from a raw pointer.
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);

            core::UniquePtr<CT> p2;

            p2 = std::move(p);
            CT_CHECK(CT::totalCtorsCalled() == 1);
            CT_CHECK(CT::assignmentsTotalCalled() == 0);
            CT_CHECK(CT::dtorsCalled() == 0);
            CT_CHECK(p.get() == nullptr);
        }
        CT_CHECK(CT::dtorsCalled() == 1);
    }

    return 0;
}

i32 releaseUniquePtrTest() {
    core::UniquePtr<i32> p = core::makeUnique<i32>(42);
    i32* raw = p.release();
    CT_CHECK(raw != nullptr);
    CT_CHECK(*raw == 42);
    CT_CHECK(p.get() == nullptr);
    core::free(raw, 1, sizeof(i32));
    return 0;
}

i32 resetUniquePtrTest() {
    core::UniquePtr<i32> p = core::makeUnique<i32>(42);
    CT_CHECK(p.get() != nullptr);
    CT_CHECK(*p.get() == 42);
    CT_CHECK(*p == 42);
    p.reset();
    CT_CHECK(p.get() == nullptr);
    return 0;
}

i32 copyUniquePtrTest() {
    core::UniquePtr<i32> p = core::makeUnique<i32>(42);
    CT_CHECK(p.get() != nullptr);
    CT_CHECK(*p.get() == 42);
    CT_CHECK(*p == 42);
    core::UniquePtr<i32> p2 = p.copy();
    CT_CHECK(p2.get() != nullptr);
    CT_CHECK(*p2.get() == 42);
    CT_CHECK(*p2 == 42);
    CT_CHECK(p.get() != p2.get());
    return 0;
}

i32 swapUniquePtrTest() {
    core::UniquePtr<i32> p1 = core::makeUnique<i32>(42);
    core::UniquePtr<i32> p2 = core::makeUnique<i32>(24);
    CT_CHECK(p1.get() != nullptr);
    CT_CHECK(p2.get() != nullptr);
    CT_CHECK(*p1.get() == 42);
    CT_CHECK(*p2.get() == 24);
    CT_CHECK(*p1 == 42);
    CT_CHECK(*p2 == 24);

    p1.swap(p2);
    CT_CHECK(p1.get() != nullptr);
    CT_CHECK(p2.get() != nullptr);
    CT_CHECK(*p1.get() == 24);
    CT_CHECK(*p2.get() == 42);
    CT_CHECK(*p1 == 24);
    CT_CHECK(*p2 == 42);
    return 0;
}

i32 runUniquePtrTestsSuite() {
    using namespace core::testing;

    auto runTests = [] (TestInfo& tInfo, const char* description, i32& retCode) {
        tInfo.description = description;

        tInfo.name = FN_NAME_TO_CPTR(initializeUniquePtrTest);
        if (runTest(tInfo, initializeUniquePtrTest) != 0) { retCode = -1; }

        tInfo.name = FN_NAME_TO_CPTR(releaseUniquePtrTest);
        if (runTest(tInfo, releaseUniquePtrTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(resetUniquePtrTest);
        if (runTest(tInfo, resetUniquePtrTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(copyUniquePtrTest);
        if (runTest(tInfo, copyUniquePtrTest) != 0) { retCode = -1; }
        tInfo.name = FN_NAME_TO_CPTR(swapUniquePtrTest);
        if (runTest(tInfo, swapUniquePtrTest) != 0) { retCode = -1; }
    };

    i32 ret = 0;
    runForAllGlobalAllocatorVariants(runTests, ret);

    {
        constexpr u32 BUFFER_SIZE = core::CORE_KILOBYTE;
        char buf[BUFFER_SIZE];
        USE_STACK_BASED_BUMP_ALLOCATOR_FOR_BLOCK_SCOPE(buf, BUFFER_SIZE);

        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        runTests(tInfo, "STACK BASED BUMP Allocator", ret);
    }

    {
        constexpr u32 BUFFER_SIZE = 256; // intentially small to test overflow.
        USE_CUSTOM_ARENA_ALLOCATOR_FOR_FOR_BLOCK_SCOPE(BUFFER_SIZE);

        TestInfo tInfo = createTestInfo();
        tInfo.trackMemory = true;
        runTests(tInfo, "CUSTOM ARENA Allocator", ret);
    }

    return ret;
}
