#include "t-index.h"

namespace {

template <core::AllocatorId TAllocId>
i32 initializeUniquePtrTest() {
    using namespace core::testing;

    {
        core::UniquePtr<i32, TAllocId> p;
        p = core::makeUnique<i32, TAllocId>(42);
        CT_CHECK(p.get() != nullptr);
        CT_CHECK(*p.get() == 42);
        CT_CHECK(*p == 42);
    }
    {
        core::UniquePtr<i32, TAllocId> p(core::makeUnique<i32, TAllocId>(42));
        CT_CHECK(p.get() != nullptr);
        CT_CHECK(*p.get() == 42);
        CT_CHECK(*p == 42);
    }
    {
        core::UniquePtr<i32, TAllocId> p = core::makeUnique<i32, TAllocId>(42);
        CT_CHECK(p.get() != nullptr);
        CT_CHECK(*p.get() == 42);
        CT_CHECK(*p == 42);
    }
    {
        auto p1 = core::makeUnique<i32, TAllocId>(42);
        auto p2 = core::makeUnique<i32, TAllocId>(24);
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
        core::expected<core::UniquePtr<i32, TAllocId>, i32> p = core::makeUnique<i32, TAllocId>(42);
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
            CT* ct = core::getAllocator(core::DEFAULT_ALLOCATOR_ID).construct<CT>();
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

template <core::AllocatorId TAllocId>
i32 releaseUniquePtrTest() {
    core::UniquePtr<i32, TAllocId> p = core::makeUnique<i32, TAllocId>(42);
    i32* raw = p.release();
    CT_CHECK(raw != nullptr);
    CT_CHECK(*raw == 42);
    CT_CHECK(p.get() == nullptr);
    core::getAllocator(TAllocId).free(raw, 1, sizeof(i32));
    return 0;
}

template <core::AllocatorId TAllocId>
i32 resetUniquePtrTest() {
    core::UniquePtr<i32, TAllocId> p = core::makeUnique<i32, TAllocId>(42);
    CT_CHECK(p.get() != nullptr);
    CT_CHECK(*p.get() == 42);
    CT_CHECK(*p == 42);
    p.reset();
    CT_CHECK(p.get() == nullptr);
    return 0;
}

template <core::AllocatorId TAllocId>
i32 copyUniquePtrTest() {
    core::UniquePtr<i32, TAllocId> p = core::makeUnique<i32, TAllocId>(42);
    CT_CHECK(p.get() != nullptr);
    CT_CHECK(*p.get() == 42);
    CT_CHECK(*p == 42);
    core::UniquePtr<i32, TAllocId> p2 = p.copy();
    CT_CHECK(p2.get() != nullptr);
    CT_CHECK(*p2.get() == 42);
    CT_CHECK(*p2 == 42);
    CT_CHECK(p.get() != p2.get());
    return 0;
}

template <core::AllocatorId TAllocId>
i32 swapUniquePtrTest() {
    core::UniquePtr<i32, TAllocId> p1 = core::makeUnique<i32, TAllocId>(42);
    core::UniquePtr<i32, TAllocId> p2 = core::makeUnique<i32, TAllocId>(24);
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

template <core::AllocatorId TAllocId>
i32 runTests() {
    using namespace core::testing;

    TestInfo tInfo = createTestInfoFor(RegisteredAllocators(TAllocId));

    defer { core::getAllocator(TAllocId).clear(); };

    tInfo.name = FN_NAME_TO_CPTR(initializeUniquePtrTest);
    if (runTest(tInfo, initializeUniquePtrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(releaseUniquePtrTest);
    if (runTest(tInfo, releaseUniquePtrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(resetUniquePtrTest);
    if (runTest(tInfo, resetUniquePtrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(copyUniquePtrTest);
    if (runTest(tInfo, copyUniquePtrTest<TAllocId>) != 0) { return -1; }
    tInfo.name = FN_NAME_TO_CPTR(swapUniquePtrTest);
    if (runTest(tInfo, swapUniquePtrTest<TAllocId>) != 0) { return -1; }

    return 0;
};

} // namespace

i32 runUniquePtrTestsSuite() {
    using namespace core::testing;

    if (runTests<RA_STD_ALLOCATOR_ID>() != 0) { return -1; }
    if (runTests<RA_STD_STATS_ALLOCATOR_ID>() != 0) { return -1; }
    if (runTests<RA_THREAD_LOCAL_BUMP_ALLOCATOR_ID>() != 0) { return -1; }
    if (runTests<RA_THREAD_LOCAL_ARENA_ALLOCATOR_ID>() != 0) { return -1; }

    constexpr u32 BUFFER_SIZE = core::CORE_KILOBYTE * 2;
    char buf[BUFFER_SIZE];
    setBufferForBumpAllocator(buf, BUFFER_SIZE);
    if (runTests<RA_BUMP_ALLOCATOR_ID>() != 0) { return -1; }

    setBlockSizeForArenaAllocator(256);
    if (runTests<RA_ARENA_ALLOCATOR_ID>() != 0) { return -1; }

    return 0;
}
